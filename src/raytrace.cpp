#include "raytrace.hpp"
#include <vector>
#include <cassert>
#include <cmath>
#include "image.hpp"
#include <map>

using namespace std;

///////////////////////////////////////////////////////////////////////////////
// static setup

static Colour AMBIENT;
static vector<Light*> LIGHTS;
static SceneNode * ROOT;
static vector<GeometryNode*> OBJECTS;

static map<string, Image*> TEXTURES;

///////////////////////////////////////////////////////////////////////////////
// headers

void get_geometry_nodes(vector<GeometryNode*> & ret, SceneNode * root);
bool light_is_visible(const Light & light, const Point3D & p);
Colour get_texture(const string & file, double x, double y);

///////////////////////////////////////////////////////////////////////////////
// main functions

void setup(SceneNode * root, const Colour & ambient, const std::list<Light*> & lights) {
  AMBIENT = ambient;
  LIGHTS.assign(lights.begin(), lights.end());

  ROOT = root;
  get_geometry_nodes(OBJECTS, root);

  root->determine_bounds();
}

Intersection2 get_colour(SceneNode * root, const Point3D & origin, const Vector3D & uray, double index) {
  assert(uray.is_unit());

  // for each object, try intersecting
  //Intersection closest = root->intersect(origin, uray);
  Intersection where[256];
  int k = root->intersections(origin, uray, CLOSEST, where);
  if (k == 0) return Intersection2();
  assert(k == 1);
  Intersection closest = where[0];

  // if an intersection has occurred, draw it
  if (closest) {
    if (PhongMaterial * pm = dynamic_cast<PhongMaterial*>(closest.material)) {
      // data we need
      //  p : the point of intersection
      //  un: surface normal at p
      //  ul: vector from p to lightsource
      //  i:  the intensity of the light
      //  ur: the perfectly-reflected ray
      // we also need to know if the light is visible from p
      // we need to make sure we aren't going the wrong way

      // the point of intersection and surface normal
      Point3D  p  = origin + closest.distance * uray;
      Vector3D un = closest.normal;
        assert(un.is_unit());
        if (un.dot(-uray) < 0) {
          cerr << "The normal indicates that the ray is looking at the back face of the object. Emergency flip.\n";
          un = -un;
        }

      // initialize the accumulator with the ambient term
      Colour display = AMBIENT * pm->get_diffuse();

      // add each light's component to the accumulator
      for (vector<Light*>::const_iterator lit = LIGHTS.begin(); lit != LIGHTS.end(); ++lit) {
        const Light & light = **lit;

        // unit vector to the light
        Vector3D ul = (light.position - p).unit();

        // skip this light if it does not hit p
        if (ul.dot(un) < 0) {
          /*cout << "behind surface\n";
          cout << "  point:  " << p << "\n"
               << "  normal: " << un << "\n"
               << "  light:  " << ul << "\n"
               << "  (dot):  " << ul.dot(un) << "\n";*/
          continue; // the light is behind the surface
        }
        if (!light_is_visible(light, p)) {
          //cout << "in shadow\n";
          continue; // p is in shadow
        }

        // compute the intensity of the light
        double i = light.intensity(sqrt((light.position - p).length()));

        // the perfectly-reflected ray
        Vector3D ur = 2 * ul.dot(un) * un - ul;

        // accumulate the diffuse component
        Colour diffuse = i * (ul.dot(un)) * pm->get_diffuse()  * light.colour;
        display = display + diffuse;

        // accumulate the specular component
        // ignore the specular if the reflected light is in the wrong direction
        double refdot = ur.dot(-uray);
        if (refdot > 0) {
          Colour specular = i * pow(refdot, pm->get_shininess()) * pm->get_specular() * light.colour;
          display = display + specular;
        }
      }

      // we have accumulated all light sources; return
      Intersection2 ret;
      ret.distance = closest.distance;
      ret.colour = display;
      return ret;
    }
    else if (TextureMaterial * tm = dynamic_cast<TextureMaterial*>(closest.material)) {
      // data we need
      //  p : the point of intersection
      //  un: surface normal at p
      //  ul: vector from p to lightsource
      //  i:  the intensity of the light
      //  ur: the perfectly-reflected ray
      // we also need to know if the light is visible from p
      // we need to make sure we aren't going the wrong way

      // the point of intersection and surface normal
      Point3D  p  = origin + closest.distance * uray;
      Vector3D un = closest.normal;
        assert(un.is_unit());
        if (un.dot(-uray) < 0) {
          cerr << "The normal indicates that the ray is looking at the back face of the object. Emergency flip.\n";
          un = -un;
        }

      // the colour of the texture
      Colour texture = get_texture(tm->get_texture_file(), closest.texture_pos[0], closest.texture_pos[1]);

      // initialize the accumulator with the ambient term
      Colour display = AMBIENT * texture;

      // add each light's component to the accumulator
      for (vector<Light*>::const_iterator lit = LIGHTS.begin(); lit != LIGHTS.end(); ++lit) {
        const Light & light = **lit;

        // unit vector to the light
        Vector3D ul = (light.position - p).unit();

        // skip this light if it does not hit p
        if (ul.dot(un) < 0) {
          continue; // the light is behind the surface
        }
        if (!light_is_visible(light, p)) {
          continue; // p is in shadow
        }

        // compute the intensity of the light
        double i = light.intensity(sqrt((light.position - p).length()));

        // the perfectly-reflected ray
        //Vector3D ur = 2 * ul.dot(un) * un - ul;

        // accumulate the diffuse component
        Colour diffuse = i * (ul.dot(un)) * texture * light.colour;
        display = display + diffuse;
      }

      // we have accumulated all light sources; return
      Intersection2 ret;
      ret.distance = closest.distance;
      ret.colour = display;
      return ret;
    }
    /*else if (OpticsMaterial * om = dynamic_cast<OpticsMaterial*>(closest.material)) {
      // the point of intersection and surface normal
      Point3D  p  = origin + closest.distance * uray;
      Vector3D un = closest.normal;
        assert(un.is_unit());
        if (un.dot(-uray) < 0) {
          cerr << "The normal indicates that the ray is looking at the back face of the object. Emergency flip.\n";
          un = -un;
        }

      // http://en.wikipedia.org/wiki/Snell's_law
      double into_idx = om->get_index();
      if (index == into_idx) into_idx = 1;

      double n1o2 = into_idx / index;
      double cos1 = un.dot(-uray);
      double cos22 = 1 - n1o2*n1o2 * (1 - cos1*cos1);

      Vector3D reflect = (uray + 2*cos1*un).unit();
      
      if (cos22 >= 0) {
        double cos2 = sqrt(cos22);
        Vector3D refract = n1o2 * uray + (n1o2 * cos1 - cos2)*un;
        return get_colour(root, p, refract, into_idx);
      }
      else {
        return get_colour(root, p, reflect);
      }
    }*/
    else {
      cerr << "Unrecognized material" << endl;
      assert(false);
    }
  }

  // there was no intersection - return the default value
  return Intersection2();
}

///////////////////////////////////////////////////////////////////////////////
// other functions

void get_geometry_nodes(vector<GeometryNode*> & ret, SceneNode * root) {
  if (GeometryNode * gn = dynamic_cast<GeometryNode*>(root)) {
    ret.push_back(gn);
  }

  for (SceneNode::ChildList::iterator it = root->children().begin(); it != root->children().end(); ++it) {
    get_geometry_nodes(ret, *it);
  }
}

bool light_is_visible(const Light & light, const Point3D & p) {
  Vector3D lv = light.position - p;
  Vector3D ul = lv.unit();
  Intersection where[256];
  int k = ROOT->intersections(p, ul, CLOSEST, where);
  //Intersection beam = ROOT->intersect(p, ul);
  if (k == 0) return 1;
  assert(k == 1);
  Intersection & beam = where[0];
  return beam.distance * beam.distance > lv.length2();
}

Colour get_texture(const string & file, double x, double y) {
  if (TEXTURES.find(file) == TEXTURES.end()) {
    Image * img = new Image();
    img->loadPng(file);
    TEXTURES[file] = img;
  }

  Image & img = *TEXTURES.at(file);
  int w = img.width();
  int h = img.height();
  int posx = x * w;
  int posy = y * h;

  Colour ret(img(posx, posy, 0),
             img(posx, posy, 1),
             img(posx, posy, 2));

  return ret;
}


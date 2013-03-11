#include "raytrace.hpp"
#include <vector>
#include <cassert>
#include <cmath>

using namespace std;

///////////////////////////////////////////////////////////////////////////////
// static setup

static Colour AMBIENT;
static vector<Light*> LIGHTS;
static SceneNode * ROOT;
static vector<GeometryNode*> OBJECTS;

///////////////////////////////////////////////////////////////////////////////
// headers

void get_geometry_nodes(vector<GeometryNode*> & ret, SceneNode * root);
bool light_is_visible(const Light & light, const Point3D & p);
Intersection get_intersection(SceneNode * root, const Point3D & origin, const Vector3D & uray);

///////////////////////////////////////////////////////////////////////////////
// main functions

void setup(SceneNode * root, const Colour & ambient, const std::list<Light*> & lights) {
  AMBIENT = ambient;
  LIGHTS.assign(lights.begin(), lights.end());

  ROOT = root;
  get_geometry_nodes(OBJECTS, root);

  root->determine_bounds();
  //root->setup_global_trans();
}

Intersection2 get_colour(SceneNode * root, const Point3D & origin, const Vector3D & uray) {
  assert(uray.is_unit());

  // for each object, try intersecting
  Intersection closest = get_intersection(root, origin, uray);

  // if an intersection has occurred, draw it
  if (closest) {
    PhongMaterial * pm = dynamic_cast<PhongMaterial*>(closest.material);
    if (!pm) {
      cerr << "BAM!\n";
      exit(1);
    }

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
        continue; // the light is behind the surface
      }
      if (!light_is_visible(light, p)) {
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
  Intersection beam = get_intersection(ROOT, p, ul);
  return beam.distance * beam.distance > lv.length2();
}

Intersection get_intersection(SceneNode * root, const Point3D & origin, const Vector3D & uray) {
  assert(uray.is_unit());

  /*
  // for each object, try intersecting
  Intersection closest;

  for (vector<GeometryNode*>::iterator it = OBJECTS.begin(); it != OBJECTS.end(); ++it) {
    Intersection candidate = (**it).intersect(origin, uray);
    if (candidate < closest) {
      closest = candidate;
    }
  }

  return closest;
  */
  return root->intersect(origin, uray);
}


#include "raytrace.hpp"
#include <vector>
#include <cassert>
#include <cmath>
#include "image.hpp"
#include <map>
#include "program_options.hpp"
#include "progress_bar.hpp"

using namespace std;

///////////////////////////////////////////////////////////////////////////////
// static setup

static SceneNode * ROOT;
static vector<GeometryNode*> OBJECTS;

static Point3D EYE;

static Colour AMBIENT;
static vector<Light*> LIGHTS;

static int WIDTH;
static int HEIGHT;
static int X_MIN;
static int X_MAX;
static int Y_MIN;
static int Y_MAX;

static UnitVector3D X(Vector3D(1,0,0)); // garbage initialization required
static UnitVector3D Y(Vector3D(0,1,0));
static UnitVector3D Z(Vector3D(0,0,1));
static double FOV_SCALE;

static map<string, Image*> TEXTURES;

static int MAX_DEPTH;
static double MIN_WEIGHT = 0.05;

static bool ENABLE_OPTICS;

///////////////////////////////////////////////////////////////////////////////
// headers

static Intersection2 get_colour(SceneNode * root, const Point3D & origin, const Vector3D & uray,
                                double index_of_refraction = 1.0, double weight = 1, double depth = 0);
static void get_geometry_nodes(vector<GeometryNode*> & ret, SceneNode * root);
static bool light_is_visible(const Light & light, const Point3D & p);
static Colour get_texture(const string & file, double x, double y);

static UnitVector3D cast_ray(int x, int y, double dx, double dy);
static Vector3D reflect(const Vector3D & incident, const Vector3D & normal);
static Colour phong_light(const Point3D & at, const Vector3D & incident, const Vector3D & normal,
                          const Colour & diffuse, const Colour & specular, double shininess);
static double fresnel_reflection(double from_idx, double to_idx, const Vector3D & incident, const Vector3D & normal);
static Vector3D refracted_ray(double from_idx, double to_idx, const Vector3D & incident, const Vector3D & normal);

///////////////////////////////////////////////////////////////////////////////
// setup

void setup(SceneNode * root, const Point3D & eye,
           const Colour & ambient, const std::list<Light*> & lights,
           int width, int height, int x_min, int x_max, int y_min, int y_max,
           UnitVector3D x, UnitVector3D y, UnitVector3D z, double fov_scale
          ) {
  ROOT = root;
  root->determine_bounds();
  get_geometry_nodes(OBJECTS, root);

  EYE = eye;

  AMBIENT = ambient;
  LIGHTS.assign(lights.begin(), lights.end());

  WIDTH = width;
  HEIGHT = height;
  X_MIN = x_min;
  X_MAX = x_max;
  Y_MIN = y_min;
  Y_MAX = y_max;

  X = x;
  Y = y;
  Z = z;
  FOV_SCALE = fov_scale;

  MAX_DEPTH = cmd_options()["max-depth"].as<int>();
  ENABLE_OPTICS = !cmd_options().count("disable-optics");
}

///////////////////////////////////////////////////////////////////////////////
// Raytrace

void raytrace(ZPic & zimg) {
  int depth = zimg.depth();
  for (int x = X_MIN; x < X_MAX; ++x) for (int y = Y_MIN; y < Y_MAX; ++y) for (int k = 0; k < depth; ++k) {
    if (y == 0) log_progress("rendering", (double)(x - X_MIN) / (X_MAX - X_MIN));

    auto & data = zimg(x, y, k);
    if (data.depth == 0) continue;

    auto ray = cast_ray(x, y, data.dx, data.dy);
    Intersection2 display = get_colour(ROOT, EYE, ray);

    if (display) {
      data.colour = display.colour;
      data.alpha  = 1;
      data.depth  = display.distance;
    }
  }
  complete_progress("rendering");
}

///////////////////////////////////////////////////////////////////////////////
// Main functions.

Intersection2 get_colour(SceneNode * root, const Point3D & origin, const Vector3D & uray,
                         double index, double weight, double depth) {
  assert(uray.is_unit());

  // return if too weak or too deep
  if (weight < MIN_WEIGHT && depth > MAX_DEPTH) return Intersection2();

  // get the closest point of intersection
  Intersection where[256];
  int k = root->intersect(origin, uray, CLOSEST, where);
  if (k == 0) return Intersection2();
  assert(k == 1);
  Intersection closest = where[0];
  assert(closest);

  // Process the point of intersection
  Point3D at          = origin + closest.distance * uray;
  Vector3D normal     = closest.normal;
    assert(normal.is_unit());
    assert(normal.dot(uray) <= 0);
  Vector3D reflected  = reflect(uray, normal);

  Colour display(0,0,0);
  double reflectivity = 0;

  // get the main colour and reflective component
  if (PhongMaterial * pm = dynamic_cast<PhongMaterial*>(closest.material)) {
    reflectivity = pm->get_reflectivity();
    display = phong_light(at, uray, normal, pm->get_diffuse(), pm->get_specular(), pm->get_shininess());
  } else if (TextureMaterial * tm = dynamic_cast<TextureMaterial*>(closest.material)) {
    Colour texture = get_texture(tm->get_texture_file(), closest.texture_pos[0], closest.texture_pos[1]);
    display = phong_light(at, uray, normal, texture, tm->get_specular() * texture, tm->get_shininess());
  } else if (OpticsMaterial * om = dynamic_cast<OpticsMaterial*>(closest.material)) {
    if (!ENABLE_OPTICS) {
      display = om->get_colour();
    } else {
      double idx = om->get_index();
      if (idx == index) idx = 1; // we are leaving the object.
      reflectivity = fresnel_reflection(index, idx, uray, normal);
      if (reflectivity < 1) {
        // some of the light is absorbed by the material
        // http://en.wikipedia.org/wiki/Opacity_(optics)
        // however, we use the constant-absorption model, as wavefronts do
        // TODO phong lighting on om->get_colour
        Vector3D refracted = refracted_ray(index, idx, uray, normal);
        double refr = 1.0 - om->get_opacity();
        auto refraction = get_colour(ROOT, at, refracted.unit(), idx, weight * (1 - reflectivity) * refr, depth + 1);
        if (refraction) {
          display = refr * refraction.colour + (1 - refr) * om->get_colour();
        } else {
          display = om->get_colour();
        }
      }
    }
  } else {
    cerr << "Unrecognized material" << endl;
    assert(false);
  }

  // add the reflective component
  // skip if weight is too low or depth is too deep
  assert(0 <= reflectivity && reflectivity <= 1);
  if (ENABLE_OPTICS && reflectivity > 0) {
    auto reflection = get_colour(ROOT, at, reflected.unit(), index, weight * reflectivity, depth + 1);
    if (reflection) display = reflectivity * reflection.colour + (1 - reflectivity) * display;
  }

  Intersection2 ret;
  ret.distance = closest.distance;
  ret.colour = display;
  return ret;
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
  if (cmd_options().count("no-shadows")) return true;

  Vector3D lv = light.position - p;
  Vector3D ul = lv.unit();
  Intersection where[256];
  int k = ROOT->intersect(p, ul, CLOSEST, where);
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

static UnitVector3D cast_ray(int x, int y, double dx, double dy) {
  // compute the ray direction for pixel (x,y)
  // note that the pixels on screen have (0,0) in the top-left, which is in the first quadrant wrt axes X and Y
  double cx = (double)WIDTH  / 2.0 - x + dx;
  double cy = (double)HEIGHT / 2.0 - y + dy;
  Vector3D ray = Z + cx * FOV_SCALE * X + cy * FOV_SCALE * Y;
  return ray;
}

Vector3D reflect(const Vector3D & incident, const Vector3D & normal) {
  return incident - 2 * incident.dot(normal) * normal;
}

Colour phong_light(const Point3D & at, const Vector3D & incident, const Vector3D & normal,
                   const Colour & diffuse, const Colour & specular, double shininess) {
  assert(normal.is_unit());
  assert(incident.is_unit());

  Colour ret(0,0,0);

  // add the ambient term
  ret += AMBIENT * diffuse;

  // add each light's term
  for (vector<Light*>::const_iterator lit = LIGHTS.begin(); lit != LIGHTS.end(); ++lit) {
    const Light & light = **lit;
    Vector3D to_light = (light.position - at).unit();
    double intensity = light.intensity(at);

    // skip this light if it is behind the normal or it is in shadow
    if (to_light.dot(normal) < 0) continue;
    if (!light_is_visible(light, at)) continue;

    // accumulate the diffuse component
    ret += intensity * to_light.dot(normal) * diffuse * light.colour;
    
    // accumulate the specular component
    // ignore the specular if the reflected light is in the wrong direction
    Vector3D reflected  = reflect(incident, normal);
    double refdot = reflected.dot(to_light);
    if (refdot > 0) ret += intensity * pow(refdot, shininess) * specular * light.colour;
  }

  return ret;
}

double fresnel_reflection(double from_idx, double to_idx, const Vector3D & _incident, const Vector3D & normal) {
  UnitVector3D incident(_incident);
  assert(normal.is_unit());

  assert(from_idx >= 1);
  assert(to_idx >= 1);

  // critical angle
  if (abs(incident.cross(normal).length()) * from_idx > to_idx) return 1;

  // compute cosines
  double cos_theta_in  = -incident.dot(normal);
  double theta_out = asin(-incident.cross(normal).length() * from_idx / to_idx);
  double cos_theta_out = cos(theta_out);

  // Fresnel Equation
  double sqrt_rs = (from_idx*cos_theta_in - to_idx*cos_theta_out) / (from_idx*cos_theta_in + to_idx*cos_theta_out);
  double sqrt_rp = (from_idx*cos_theta_out - to_idx*cos_theta_in) / (from_idx*cos_theta_out + to_idx*cos_theta_in);
  
  double rs = sqrt_rs * sqrt_rs;
  double rp = sqrt_rp * sqrt_rp;

  double ret = (rs + rp) / 2;

  assert(0 < ret && ret < 1);
  return ret;
}

Vector3D refracted_ray(double from_idx, double to_idx, const Vector3D & _incident, const Vector3D & normal) {
  // http://graphics.stanford.edu/courses/cs148-10-summer/docs/2006--degreve--reflection_refraction.pdf
  UnitVector3D incident(_incident);
  assert(normal.is_unit());

  assert(from_idx >= 1);
  assert(to_idx >= 1);

  double cos_in = -incident.dot(normal);
  double nr = from_idx / to_idx;
  double sin2out = nr*nr * (1 - cos_in*cos_in);

  Vector3D ret = nr*incident + (nr*cos_in - sqrt(1 - sin2out))*normal;

  assert(ret.dot(normal) < 0);
  return ret;
}


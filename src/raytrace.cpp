#include "raytrace.hpp"
#include <vector>

using namespace std;

///////////////////////////////////////////////////////////////////////////////
// static setup

static bool INITIALIZED = false;

// lighting
static Colour AMBIENT;
static vector<Light*> LIGHTS;

// scene
static vector<GeometryNode*> OBJECTS;

///////////////////////////////////////////////////////////////////////////////
// headers

void get_geometry_nodes(vector<GeometryNode*> & ret, SceneNode * root);

///////////////////////////////////////////////////////////////////////////////
// main functions

void setup(SceneNode * root, const Colour & ambient, const std::list<Light*> & lights) {
  AMBIENT = ambient;
  LIGHTS.assign(lights.begin(), lights.end());

  get_geometry_nodes(OBJECTS, root);
  
  INITIALIZED = true;
}

Intersection2 get_colour(SceneNode * root, const Point3D & origin, const Vector3D & uray) {
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


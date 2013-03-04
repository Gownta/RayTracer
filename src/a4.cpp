#include "a4.hpp"
#include "image.hpp"
#include <cmath>

using namespace std;

struct Intersection {
  SceneNode * object;
  double distance;

  Intersection() : object(NULL), distance(0.0) {}
  Intersection(SceneNode * object, double distance) : object(object), distance(distance) {}

  operator bool() const { return object != NULL; }
};

Intersection trace(SceneNode * root, const Point3D & eye, const Vector3D & ray);

///////////////////////////////////////////////////////////////////////////////

void a4_render(// What to render
               SceneNode* root,
               // Where to output the image
               const std::string& filename,
               // Image size
               int width, int height,
               // Viewing parameters
               const Point3D& eye, const Vector3D& view,
               const Vector3D& up, double fov,
               // Lighting parameters
               const Colour& ambient,
               const std::list<Light*>& lights
               )
{
  /////////////////////////////////////
  // Log and massage.

  if (true) {
    cerr << "rendering: scene       " << root << "\n"
         << "           to          " << filename << "\n"
         << "           size        " << "(" << width << ", " << height << ")\n"
         << "           eye         " << eye << "\n"
         << "           view        " << view << "\n"
         << "           up          " << up << "\n"
         << "           fov         " << fov << "\n"
         << "           ambient     " << ambient << "\n"
         << "           lights:\n";

    for (std::list<Light*>::const_iterator I = lights.begin(); I != lights.end(); ++I)
    cerr << "               " << **I << "\n";

    cerr << endl;
  }

  // compute the left axis
  Vector3D left = up.cross(view);

  // normalize all axes
  double view_len = view.length();
  double up_len   = up  .length();
  double left_len = left.length();

  const Vector3D X = (1/left_len)*left;
  const Vector3D Y = (1/up_len)  *up;
  const Vector3D Z = (1/view_len)*view;

  // find the field of view scaling factor
  double fov_scale = tan((fov/2) * 2*M_PI / 360);
  
  /////////////////////////////////////
  // The image. The 3 is for RGB.
  Image img(width, height, 3);

  /////////////////////////////////////
  // Construct the background of the image.

  // TODO: improve

  // Currently: red-blue checkerboard.
  const int len = 32;
  for (int x = 0; x < width; ++x) for (int y = 0; y < height; ++y) {
    int box_x = x / len;
    int box_y = y / len;
    
    bool red = (box_x + box_y) % 2;

    // RGB
    img(x, y, 0) = (double)red / 2;
    img(x, y, 1) = 0;
    img(x, y, 2) = (double)(1 - red) / 2;
  }

  /////////////////////////////////////
  // Raytracing.

  for (int x = 0; x < width; ++x) for (int y = 0; y < height; ++y) {
    // compute the ray direction for pixel (x,y)
    // note that the pixels on screen have (0,0) in the top-left, which is in the first quadrant wrt axes X and Y
    double cx = (double)width / 2.0  - x;
    double cy = (double)height / 2.0 - y;
    Vector3D ray = Z + cx * fov_scale * X + cy * fov_scale * Y;

    
    
    // does the ray from the eye to each sphere intersect the sphere?
  }

  /////////////////////////////////////
  // Save the image.
  img.savePng(filename);
  
}

///////////////////////////////////////////////////////////////////////////////

Intersection trace(SceneNode * root, const Point3D & eye, const Vector3D & ray) {
  return Intersection();
}


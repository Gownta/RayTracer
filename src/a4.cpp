#include "a4.hpp"
#include "image.hpp"
#include <cmath>
#include "raytrace.hpp"
#include <ctime>
#include "progress_bar.hpp"
#include "background.hpp"

using namespace std;

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
  // Setup.

  // compute the left axis
  Vector3D left = up.cross(view);

  // normalize all axes
  const UnitVector3D X = left;
  const UnitVector3D Y = up;
  const UnitVector3D Z = view;

  // find the field of view scaling factor
  // this factor is over the width of the image
  double fov_scale = tan((fov/2) * 2*M_PI / 360) / (width / 2);
  
  // Initialize the image. The 3 is for RGB.
  Image img(width, height, 3);
  set_background(img);

  /////////////////////////////////////
  // Raytracing.

  setup(root, ambient, lights);

  for (int x = 0; x < width; ++x) for (int y = 0; y < height; ++y) {
  //for (int x = 128; x < 129; ++x) for (int y = 128; y < 129; ++y) {
    // compute the ray direction for pixel (x,y)
    // note that the pixels on screen have (0,0) in the top-left, which is in the first quadrant wrt axes X and Y
    double cx = (double)width / 2.0  - (x + 0.5);
    double cy = (double)height / 2.0 - (y + 0.5);

    if (y == 0) log_progress("bananas", (double)x / width);

    Vector3D ray = (Z + cx * fov_scale * X + cy * fov_scale * Y).unit();
    Intersection2 display = get_colour(root, eye, ray);

    if (display) {
      img(x, y, 0) = display.colour.R();
      img(x, y, 1) = display.colour.G();
      img(x, y, 2) = display.colour.B();
    }
  }

  /////////////////////////////////////
  // Save the image.
  //img.savePng(filename);
  img.savePng("scene.png");
}


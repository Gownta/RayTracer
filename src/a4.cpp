#include "a4.hpp"
#include "image.hpp"
#include <cmath>
#include "raytrace.hpp"
#include <ctime>
#include "progress_bar.hpp"
#include "background.hpp"
#include "program_options.hpp"

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

  // determine which range of the image to render
  int x_min = 0;
  int x_max = width;
  int y_min = 0;
  int y_max = height;
  if (cmd_options().count("x-range")) {
    vector<int> xr = cmd_options()["x-range"].as<vector<int>>();
    if (xr.size() >= 1) x_min = xr[0];
    if (xr.size() >= 2) x_max = xr[1];
  }
  if (cmd_options().count("y-range")) {
    vector<int> yr = cmd_options()["y-range"].as<vector<int>>();
    if (yr.size() >= 1) y_min = yr[0];
    if (yr.size() >= 2) y_max = yr[1];
  }

  /////////////////////////////////////
  // Raytracing.

  setup(root, ambient, lights);

  for (int x = x_min; x < x_max; ++x) for (int y = y_min; y < y_max; ++y) {
    // compute the ray direction for pixel (x,y)
    // note that the pixels on screen have (0,0) in the top-left, which is in the first quadrant wrt axes X and Y
    double cx = (double)width / 2.0  - (x + 0.5);
    double cy = (double)height / 2.0 - (y + 0.5);

    if (y == 0) log_progress("rendering", (double)x / width);

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

  if (cmd_options().count("altfile")) {
    img.savePng(filename);
  } else {
    img.savePng("scene.png");
  }
}


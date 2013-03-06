#include "a4.hpp"
#include "image.hpp"
#include <cmath>
#include "raytrace.hpp"

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

  // TODO delnow
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
  const Vector3D X = left.unit();
  const Vector3D Y = up  .unit();
  const Vector3D Z = view.unit();

  // find the field of view scaling factor
  // this factor is over the width of the image
  double fov_scale = tan((fov/2) * 2*M_PI / 360) / (width / 2);
  
  // Initialize the image. The 3 is for RGB.
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

    img(x, y, 0) = (double)red / 2;
    img(x, y, 1) = 0;
    img(x, y, 2) = (double)(1 - red) / 2;
  }

  /////////////////////////////////////
  // Raytracing.

  setup(root, ambient, lights);

  for (int x = 0; x < width; ++x) for (int y = 0; y < height; ++y) {
  //for (int x = 160; x < 192; ++x) for (int y = 96; y < 128; ++y) {
    // compute the ray direction for pixel (x,y)
    // note that the pixels on screen have (0,0) in the top-left, which is in the first quadrant wrt axes X and Y
    double cx = (double)width / 2.0  - x;
    double cy = (double)height / 2.0 - y;
    Vector3D ray = (Z + cx * fov_scale * X + cy * fov_scale * Y).unit();

    Intersection2 display = get_colour(root, eye, ray);

    if (display) {
      img(x, y, 0) = display.colour.R();
      img(x, y, 1) = display.colour.G();
      img(x, y, 2) = display.colour.B();
    }
  }

    /*
    // for each object, try intersecting
    Intersection closest;

    for (vector<GeometryNode*>::iterator it = objs.begin(); it != objs.end(); ++it) {
      Intersection candidate = (**it).intersect(eye, ray);
      if (candidate < closest) {
        closest = candidate;
      }
    }

    // if an intersection has occurred, draw it
    if (closest) {
      PhongMaterial * pm = dynamic_cast<PhongMaterial*>(closest.material);
      if (!pm) {
        cerr << "BAM!\n";
        exit(1);
      }

      Colour display = ambient * pm->get_diffuse();

      // for each light source, determine if it is visible
      // if it is, phong light it
      for (list<Light*>::const_iterator lit = lights.begin(); lit != lights.end(); ++lit) {
        const Light & light = **lit;
        Point3D at = eye + closest.distance * ray;
        Vector3D l = light.position - at;
        l.normalize();

        double d2l = sqrt((light.position - at).length());

        bool visible = true;
        for (vector<GeometryNode*>::iterator it = objs.begin(); it != objs.end(); ++it) {
          Intersection candidate = (**it).intersect(at, l);
          if (candidate.distance < d2l) {
            visible = false;
            break;
          }
        }
        if (!visible) continue;

        double intensity = 1 / (light.falloff[0] + light.falloff[1]*d2l + light.falloff[2]*d2l*d2l);
        double into = l.dot(closest.normal);
        if (into < 0) continue; // the light is behind the surface
        Vector3D reflect = 2*l.dot(closest.normal)*closest.normal - l;
        double spec = pow(reflect.dot(-ray), pm->get_shininess());

        Colour diffuse  = intensity * into * pm->get_diffuse()  * light.colour;
        Colour specular = intensity * spec * pm->get_specular() * light.colour;

        display = display + diffuse + specular;

      }

      img(x, y, 0) = display.R();
      img(x, y, 1) = display.G();
      img(x, y, 2) = display.B();
      //img(x, y, 0) = pm->get_diffuse().R();
      //img(x, y, 1) = pm->get_diffuse().G();
      //img(x, y, 2) = pm->get_diffuse().B();
      //img(x, y, 0) = closest.material->get_diffuse().R();
      //img(x, y, 1) = closest.material->get_diffuse().G();
      //img(x, y, 2) = closest.material->get_diffuse().B();
    }
    */

  /////////////////////////////////////
  // Save the image.
  img.savePng(filename);
  
}


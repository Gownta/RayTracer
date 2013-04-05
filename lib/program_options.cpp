#include "program_options.hpp"
#include <iostream>

using namespace std;

static po::options_description desc("RayTracer Options");
static po::variables_map vm;

po::variables_map & cmd_options() {
  return vm;
}

void read_options(int argc, char ** argv) {
  //
  // The options
  //
  desc.add_options()
      // main
      ("help,h", "Display help message.")

      // image options
      ("background,b", po::value<int>()->default_value(2), "Background image selector.")
      ("x-range,x", po::value<vector<int>>()->multitoken(), "Range of x-coordinates to draw.")
      ("y-range,y", po::value<vector<int>>()->multitoken(), "Range of y-coordinates to draw.")
      ("no-shadows,S", "Disable shadows: all lights are assumed to hit every surface.")
      ("disable-optics", "Disable reflections and refractions")

      // anti-aliasing
      ("anti-aliasing,A", po::value<double>()->default_value(0.01), "Variance above which anti-aliasing occurs.")
      ("alias-metric,v", po::value<int>()->default_value(0), "Anti-aliasing selection selector.")
      ("alias-method,a", po::value<int>()->default_value(1), "Anti-aliasing resolution selector.")
      ("show-anti-aliasing", "Highlight pixels that need anti-aliasing in red.")
      ("enable-anti-aliasing", "Enable anti-aliasing.")

      // program options
      ("altfile,F", "Output the png to the render file instead of scene.png.")
      ("reverse,R", "Use matrix post-order composition instead of pre-order.")
      ("average-origin-spheres", "Use average-origin spheres to construct bounding regions.")
      ("quiet,Q", "Disable progress bars.")
      ("max-depth", po::value<int>()->default_value(3), "Maximum number of reflections/refractions.")
  ;

  // positional options
  po::positional_options_description pos;
  pos.add("source", 1);

  po::options_description dpos("Positionals");
  dpos.add_options()
      ("source", po::value<string>()->default_value("scene.lua"), "lua source file")
  ;

  // parse the command line
  po::options_description all;
  all.add(desc).add(dpos);

  po::store(po::command_line_parser(argc, argv).options(all).positional(pos).run(), vm);
  po::notify(vm);
}

void print_options() {
  cout << desc << endl;
}


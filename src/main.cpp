#include <iostream>
#include "scene_lua.hpp"

using namespace std;

int main(int argc, char ** argv) {
  // Get the lua scene filename.
  string filename = "scene.lua";
  if (argc >= 2) filename = argv[1];

  // Execute the lua scene.
  if (!run_lua(filename)) {
    cerr << "Failed to execute lua scene " << filename << endl;
    return 1;
  }

  return 0;
}


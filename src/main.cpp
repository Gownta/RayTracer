#include <iostream>
#include "program_options.hpp"
#include "scene_lua.hpp"
using namespace std;

int main(int argc, char ** argv) {
  read_options(argc, argv);

  // Help
  if (cmd_options().count("help")) {
    cout << "\n     ===== RayTracer =====\n\n"
              "     Nicholas Ormrod (njormrod)\n"
              "     CS 488 A5, W12\n"
            "\n     =====================\n\n";

    cout << "Usage: rt [options] [filename]\n\n";

    print_options();

    return 1;
  }

  // Execute the lua scene.
  string filename = cmd_options()["source"].as<string>();
  if (!run_lua(filename)) {
    cerr << "Failed to execute lua scene " << filename << endl;
    return 1;
  }

  return 0;
}


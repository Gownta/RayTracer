#include <iostream>
#include "program_options.hpp"
#include "scene_lua.hpp"
#include <cstdlib>
using namespace std;

/*#include "algorithms.hpp"
#include <iomanip>
void outr() {
  cout << setprecision(10);
  for (int i = 0; i < 1000; ++i) {
    Vector3D r = random_normal();
    cout << "{ " << r[0] << ", " << r[1] << ", " << r[2] << " },\n";
  }
}*/

int main(int argc, char ** argv) {
  srand(time(NULL));

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


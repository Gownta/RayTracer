#include "progress_bar.hpp"
#include "program_options.hpp"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <ios>
#include <ctime>

using namespace std;

static clock_t UNIVERSAL_START = -1;
static clock_t LAST_START;
static string  LAST_MESSAGE("garbage sjkghasdgjcgnfkj");
static int     LAST_GREEN;
static int     LAST_ELAPSED = -1;

void log_progress(const std::string & message, double progress, bool local_time) {
  if (cmd_options().count("quiet")) return;
  if (message != LAST_MESSAGE) {
    LAST_START = clock();
    LAST_MESSAGE = message;
    LAST_GREEN = -1;

    if (UNIVERSAL_START == -1) UNIVERSAL_START = LAST_START;
  }

  int elapsed = (clock() - (local_time ? LAST_START : UNIVERSAL_START)) / CLOCKS_PER_SEC;
  
  static string g("\x1b[1;;42m"); // green
  static string n("\x1b[0m");     // reset
  static string b = "                                                                                ";

  int green = 80 * progress;
  assert(0 <= green && green <= 80);

  if (green == LAST_GREEN && elapsed == LAST_ELAPSED) return;
  LAST_GREEN = green;
  LAST_ELAPSED = elapsed;
  
  // Compute the progress bar message
  stringstream display;
  display << setw(70) << left << message;
  
  stringstream time;
  time << setfill('0') << right;
  if (elapsed >= 60*60) {
    int hours = elapsed / 60 / 60;
    time << hours << ":" << setw(2);
  }
  if (elapsed >= 60) {
    int minutes = (elapsed / 60) % 60;
    time << minutes << ":" << setw(2);
  }
  if (elapsed >= 0) {
    int seconds = elapsed % 60;
    time << seconds;
  }

  display << setw(8) << right << time.str() << "s" << "  ";
  string ds = display.str();
  assert(ds.length() > 80);

  // display
  cout << b << "\r";
  cout << g << ds.substr(0, green);
  cout << n << ds.substr(green);
  cout << "\r";
  cout << flush;
}

void complete_progress(const string & msg) {
  if (cmd_options().count("quiet")) return;
  LAST_GREEN = -1;
  log_progress(msg, 1, true);
  cout << endl;
}

void wipe_line() {
  if (cmd_options().count("quiet")) return;
  cout << "                                                                                 \r";
}


#include "progress_bar.hpp"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <ios>
#include <ctime>

using namespace std;

static clock_t START;
static string LAST_MESSAGE;
static int    LAST_GREEN;
static int    LAST_ELAPSED = -1;

struct RUNNER {
  RUNNER() { START = clock(); }
  ~RUNNER() { cout << endl; }
} runner;

void log_progress(const std::string & message, double progress) {
  int elapsed = (clock() - START) / CLOCKS_PER_SEC;
  
  static string g("\x1b[1;;42m"); // green
  static string n("\x1b[0m");     // reset
  static string b = "                                                                                ";

  int green = 80 * progress;

  if (green == LAST_GREEN && message == LAST_MESSAGE && elapsed == LAST_ELAPSED) return;
  LAST_GREEN = green;
  if (message != LAST_MESSAGE) LAST_MESSAGE = message;
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

  display << setw(8) << right << time.str() << "s";
  string ds = display.str();

  // display
  cout << b << "\r";
  cout << g << ds.substr(0, green);
  cout << n << ds.substr(green);
  cout << "\r";
  cout << flush;
}


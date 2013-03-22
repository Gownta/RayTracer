#ifndef GOWNTA_PROGRAM_OPTIONS_H
#define GOWNTA_PROGRAM_OPTIONS_H

#include <boost/program_options.hpp>

namespace po = boost::program_options;

po::variables_map & cmd_options();
void read_options(int argc, char ** argv);
void print_options();

#endif // GOWNTA_PROGRAM_OPTIONS_H


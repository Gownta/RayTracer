#ifndef GOWNTA_CALC_H
#define GOWNTA_CALC_H

#include <iostream>
#include <string>
#include <vector>

struct Factor {
  Factor() : k(), pow_t(), pow_ox(), pow_oy(), pow_oz(), pow_rx(), pow_ry(), pow_rz(), pow_x(), pow_y(), pow_z() {}
  double k;
  // powers of t, origin[0..2], ray[0..2]
  int pow_t;
  int pow_ox;
  int pow_oy;
  int pow_oz;
  int pow_rx;
  int pow_ry;
  int pow_rz;

  int pow_x;
  int pow_y;
  int pow_z;
};

std::ostream & operator<<(std::ostream & os, const Factor & f);
std::vector<Factor> parse_equation(const std::string & eqn, bool expant_t);

#endif // GOWNTA_CALC_H


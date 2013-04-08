#include "antialias.hpp"
#include "program_options.hpp"
#include <vector>
#include "progress_bar.hpp"
#include "raytrace.hpp"
#include <cstdlib>

using namespace std;

///////////////////////////////////////////////////////////////////////////////
// constructor

AntiAliaser::AntiAliaser(ZPic & zimg, int x_min, int x_max, int y_min, int y_max)
    : m_zimg(zimg)
    , m_x_min(x_min)
    , m_x_max(x_max)
    , m_y_min(y_min)
    , m_y_max(y_max)
{
  int metric = cmd_options()["alias-metric"].as<int>();
  m_max = metric % 2;
  if (metric / 2 == 1) {
    // expanded cross
    m_aliasing.emplace_back( 1, 0, 1);
    m_aliasing.emplace_back(-1, 0, 1);
    m_aliasing.emplace_back( 0, 1, 1);
    m_aliasing.emplace_back( 0,-1, 1);

    m_aliasing.emplace_back( 1, 1, 1);
    m_aliasing.emplace_back( 1,-1, 1);
    m_aliasing.emplace_back(-1, 1, 1);
    m_aliasing.emplace_back(-1,-1, 1);

    m_aliasing.emplace_back( 2, 0, 0.5);
    m_aliasing.emplace_back(-2, 0, 0.5);
    m_aliasing.emplace_back( 0, 2, 0.5);
    m_aliasing.emplace_back( 0,-2, 0.5);
  } else {
    // cross
    m_aliasing.emplace_back( 1, 0, 1);
    m_aliasing.emplace_back(-1, 0, 1);
    m_aliasing.emplace_back( 0, 1, 1);
    m_aliasing.emplace_back( 0,-1, 1);
  }

  m_cutoff = cmd_options()["anti-aliasing"].as<double>();
}

///////////////////////////////////////////////////////////////////////////////
// deciders

double AntiAliaser::colour_var(const Colour & c1, const Colour & c2) const {
  double ret = 0;
  for (int i = 0; i < 3; ++i) {
    double delta = c1[i] - c2[i];
    ret += delta * delta;
  }
  return ret;
}

bool AntiAliaser::needs_aliasing(int x, int y) const {
  // compute the variance average/maximum
  double top = 0;
  double tot = 0;
  double weight = 0;

  for (const auto & offset : m_aliasing) {
    int ox = x + offset.dx;
    int oy = y + offset.dy;

    if (   ox <  m_x_min
        || ox >= m_x_max
        || oy <  m_y_min
        || oy >= m_y_max) continue;

    double var = colour_var(m_zimg(x,y).colour, m_zimg(ox,oy).colour);
    double wv = var * offset.weight;

    top = max(top, wv);
    tot += wv;
    weight += offset.weight;
  }

  // does the pixel need anti-aliasing
  double val;
  if      (m_max)       val = top;
  else if (weight == 0) val = 0;
  else                  val = tot / weight;

  return val > m_cutoff;
}

// generate a random number in the range [-0.5, 0.5)
static double randoff() {
  return (rand() % 1048576) / 1048576.0 - 0.5;
}

///////////////////////////////////////////////////////////////////////////////
// perform anti-aliasing

void AntiAliaser::antialias() {
  if (!cmd_options().count("enable-anti-aliasing")) return;
  srand(time(NULL));

  // determine which pixels need anti-aliasing
  bool aliased[m_x_max - m_x_min][m_y_max - m_y_min];
  int aliasing_count = 0;
  for (int x = m_x_min; x < m_x_max; ++x) for (int y = m_y_min; y < m_y_max; ++y) {
    if (y == m_y_min) log_progress("anti-aliasing selection", (double)(x - m_x_min) / (m_x_max - m_x_min) / 2);
    bool na = needs_aliasing(x, y);
    aliased[x - m_x_min][y - m_y_min] = na;
    aliasing_count += na;
  }

  // log anti-aliasing details
  wipe_line();
  int pixels = (m_x_max - m_x_min) * (m_y_max - m_y_min);
  cout << aliasing_count << " / " << pixels
       << " pixels (" << ((int)((double)aliasing_count / pixels * 100)) << "%) need anti-aliasing.\n";

  // red outline instead of actual anti-aliasing
  if (cmd_options().count("show-anti-aliasing")) {
    for (int x = m_x_min; x < m_x_max; ++x) for (int y = m_y_min; y < m_y_max; ++y) {
      if (aliased[x - m_x_min][y - m_y_min]) {
        m_zimg(x,y).colour = Colour(1,0,0);
        m_zimg(x,y).depth  = 0;
        m_zimg(x,y).alpha  = 1;
      }
    }
    complete_progress("anti-aliasing selection");
    return;
  }

  // initialize a deep ZPic for anti-aliasing
  int depth = 9;
  int sqrtd = sqrt(depth);
  ZPic xzimg(m_zimg.width(), m_zimg.height(), depth);

  int resolution = cmd_options()["alias-method"].as<int>();

  for (int x = m_x_min; x < m_x_max; ++x) for (int y = m_y_min; y < m_y_max; ++y) {
    if (y == m_y_min) log_progress("anti-aliasing", (double)(x - m_x_min) / (m_x_max - m_x_min) / 2 + 0.5);
    if (aliased[x - m_x_min][y - m_y_min]) {
      if (resolution == 3) {
        // random
        for (int i = 0; i < depth; ++i) {
          double dx = randoff() + 0.5;
          double dy = randoff() + 0.5;

          xzimg(x,y,i).dx = dx;
          xzimg(x,y,i).dy = dy;
        }
      } else {
        // grid
        for (int i = 0; i < depth; ++i) {
          int ix = i / sqrtd;
          int iy = i % sqrtd;

          if (resolution == 2) {
            // jittered grid
            ix += randoff() / sqrtd;
            iy += randoff() / sqrtd;
          }

          xzimg(x,y,i).dx = (2.0*ix + 1) / (2.0 * sqrtd);
          xzimg(x,y,i).dy = (2.0*iy + 1) / (2.0 * sqrtd);
        }
      }
    } else {
      for (int i = 0; i < depth; ++i) xzimg(x,y,i).depth = 0;
    }
  }
  complete_progress("anti-aliasing selection");

  // render the deep image
  raytrace(xzimg);

  // merge the deep image
  for (int x = m_x_min; x < m_x_max; ++x) for (int y = m_y_min; y < m_y_max; ++y) {
    if (y == m_y_min) log_progress("merging anti-aliased image", (double)(x - m_x_min) / (m_x_max - m_x_min));
    if (aliased[x - m_x_min][y - m_y_min]) {
      Colour front(0,0,0);
      int hits = 0;
      for (int k = 0; k < depth; ++k) {
        auto & data = xzimg(x,y,k);
        if (data.depth < INFINITY) {
          assert(data.alpha == 1);
          front = front + data.colour;
          hits++;
        }
      }
      if (hits) m_zimg(x,y).colour = (1.0 / hits) * front;
      m_zimg(x,y).alpha  = (1.0 / depth) * hits;
      m_zimg(x,y).depth  = -1; // some garbage non-INFINITY value
    }
  }
  complete_progress("merging anti-aliased image");
}


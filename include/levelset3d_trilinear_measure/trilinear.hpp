#pragma once

#include <array>
#include <vector>

#include "common_geometry/types.hpp"

namespace ns_ls3tm {

// Cube corner offsets in the standard marching-cubes / Lorensen-Cline
// convention (matches levelset3d_polygon's own kCornerOffset): corner
// c's offset from a cell's local origin, in {0,1}^3.
//
//      4---------5
//     /|        /|
//    7---------6 |
//    | |       | |
//    | 0-------|-1
//    |/        |/
//    3---------2
inline constexpr std::array<std::array<int, 3>, 8> kCubeCornerOffset = {{
    {0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0},
    {0, 0, 1}, {1, 0, 1}, {1, 1, 1}, {0, 1, 1},
}};

// Trilinear interpolation of 8 corner values `v` (ordered per
// kCubeCornerOffset) at local coordinates (x,y,z), each typically in
// [0,1] (values outside that range extrapolate the same multilinear
// function -- there's nothing special about the unit range here).
inline double TrilinearValue(const std::array<double, 8>& v, double x,
                              double y, double z) {
  double result = 0.0;
  for (int c = 0; c < 8; ++c) {
    const auto& o = kCubeCornerOffset[c];
    const double wx = o[0] ? x : 1.0 - x;
    const double wy = o[1] ? y : 1.0 - y;
    const double wz = o[2] ? z : 1.0 - z;
    result += v[c] * wx * wy * wz;
  }
  return result;
}

// Finds every parametric t in [0,1] where the trilinear interpolant of
// `v` (local cube coordinates, see TrilinearValue) crosses zero along
// the line origin + t*dir. Works for any line direction: restricted to
// a line, the trilinear interpolant is a cubic in t in general (linear
// only for an axis-aligned line with the other two coordinates fixed),
// so this doesn't assume a closed form -- it's a dense-sampling +
// bisection root find instead.
inline std::vector<double> FindTrilinearCrossings(const std::array<double, 8>& v,
                                                    const ns_cg::Vec3d& origin,
                                                    const ns_cg::Vec3d& dir,
                                                    int samples = 4000) {
  const auto f = [&](double t) {
    const ns_cg::Vec3d p = origin + t * dir;
    return TrilinearValue(v, p.x(), p.y(), p.z());
  };

  std::vector<double> roots;
  double prev_t = 0.0;
  double prev_f = f(0.0);
  if (prev_f == 0.0) roots.push_back(0.0);
  for (int i = 1; i <= samples; ++i) {
    const double t = static_cast<double>(i) / samples;
    const double fv = f(t);
    if ((prev_f < 0.0) != (fv < 0.0)) {
      double lo = prev_t, hi = t, flo = prev_f;
      for (int iter = 0; iter < 60; ++iter) {
        const double mid = 0.5 * (lo + hi);
        const double fm = f(mid);
        if ((fm < 0.0) == (flo < 0.0)) {
          lo = mid;
          flo = fm;
        } else {
          hi = mid;
        }
      }
      roots.push_back(0.5 * (lo + hi));
    }
    prev_t = t;
    prev_f = fv;
  }
  return roots;
}

}  // namespace ns_ls3tm

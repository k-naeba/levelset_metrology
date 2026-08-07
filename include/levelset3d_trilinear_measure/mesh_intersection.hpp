#pragma once

#include <algorithm>
#include <cmath>
#include <optional>
#include <vector>

#include "common_geometry/mesh3d.hpp"
#include "common_geometry/types.hpp"

namespace ns_ls3tm {

// Standard Moller-Trumbore ray-triangle intersection: returns the ray
// parameter t (origin + t*dir) where the segment crosses triangle
// (a,b,c), or nullopt if it doesn't (including the near-parallel and
// outside-the-triangle cases).
inline std::optional<double> RayTriangleIntersect(const ns_cg::Vec3d& origin,
                                                    const ns_cg::Vec3d& dir,
                                                    const ns_cg::Vec3d& a,
                                                    const ns_cg::Vec3d& b,
                                                    const ns_cg::Vec3d& c) {
  constexpr double kEps = 1e-12;
  const ns_cg::Vec3d edge1 = b - a;
  const ns_cg::Vec3d edge2 = c - a;
  const ns_cg::Vec3d pvec = dir.cross(edge2);
  const double det = edge1.dot(pvec);
  if (std::abs(det) < kEps) return std::nullopt;
  const double inv_det = 1.0 / det;

  const ns_cg::Vec3d tvec = origin - a;
  const double u = tvec.dot(pvec) * inv_det;
  if (u < 0.0 || u > 1.0) return std::nullopt;

  const ns_cg::Vec3d qvec = tvec.cross(edge1);
  const double w = dir.dot(qvec) * inv_det;
  if (w < 0.0 || u + w > 1.0) return std::nullopt;

  return edge2.dot(qvec) * inv_det;
}

// Finds every t in [0,1] where the segment origin + t*dir crosses one of
// mesh's triangles, sorted ascending. A line grazing an edge shared by
// two triangles may be reported twice (once per triangle); callers
// wanting a deduplicated crossing count should account for that (e.g. by
// merging hits closer together than a small epsilon).
inline std::vector<double> FindMeshCrossings(const ns_cg::Mesh3d& mesh,
                                              const ns_cg::Vec3d& origin,
                                              const ns_cg::Vec3d& dir) {
  std::vector<double> hits;
  for (const auto& tri : mesh.GetTriangles()) {
    const auto t = RayTriangleIntersect(
        origin, dir, mesh.GetVertices()[tri[0]], mesh.GetVertices()[tri[1]],
        mesh.GetVertices()[tri[2]]);
    if (t.has_value() && *t >= -1e-9 && *t <= 1.0 + 1e-9) hits.push_back(*t);
  }
  std::sort(hits.begin(), hits.end());
  return hits;
}

}  // namespace ns_ls3tm

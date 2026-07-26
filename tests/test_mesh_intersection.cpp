#include <gtest/gtest.h>

#include "levelset_metrology/mesh_intersection.hpp"

namespace ns_lm {
namespace {

using ns_cg::Mesh3d;
using ns_cg::Vec3d;

TEST(RayTriangleIntersectTest, HitsTriangleCenterFromAbove) {
  const Vec3d a(0, 0, 0), b(2, 0, 0), c(0, 2, 0);
  const auto t = RayTriangleIntersect(Vec3d(0.5, 0.5, 5.0), Vec3d(0, 0, -1),
                                       a, b, c);
  ASSERT_TRUE(t.has_value());
  EXPECT_NEAR(*t, 5.0, 1e-9);
}

TEST(RayTriangleIntersectTest, MissesOutsideTriangleBounds) {
  const Vec3d a(0, 0, 0), b(2, 0, 0), c(0, 2, 0);
  const auto t = RayTriangleIntersect(Vec3d(5.0, 5.0, 5.0), Vec3d(0, 0, -1),
                                       a, b, c);
  EXPECT_FALSE(t.has_value());
}

TEST(RayTriangleIntersectTest, ParallelToPlaneMisses) {
  const Vec3d a(0, 0, 0), b(2, 0, 0), c(0, 2, 0);
  const auto t = RayTriangleIntersect(Vec3d(0.5, 0.5, 1.0), Vec3d(1, 0, 0), a,
                                       b, c);
  EXPECT_FALSE(t.has_value());
}

TEST(FindMeshCrossingsTest, LineThroughAQuadCrossesOnce) {
  // A flat quad (z=0, spanning [0,2]x[0,2]) made of two triangles,
  // split along the (0,0)-(2,2) diagonal. (1.5, 0.5) is inside triangle
  // (0,1,2) only, well clear of that shared diagonal.
  Mesh3d mesh({Vec3d(0, 0, 0), Vec3d(2, 0, 0), Vec3d(2, 2, 0), Vec3d(0, 2, 0)},
              {{0, 1, 2}, {0, 2, 3}});

  const std::vector<double> hits =
      FindMeshCrossings(mesh, Vec3d(1.5, 0.5, -5.0), Vec3d(0, 0, 10.0));
  ASSERT_EQ(hits.size(), 1u);
  EXPECT_NEAR(hits[0], 0.5, 1e-9);  // crosses z=0 exactly halfway
}

TEST(FindMeshCrossingsTest, LineMissingTheQuadHasNoCrossings) {
  Mesh3d mesh({Vec3d(0, 0, 0), Vec3d(2, 0, 0), Vec3d(2, 2, 0), Vec3d(0, 2, 0)},
              {{0, 1, 2}, {0, 2, 3}});

  const std::vector<double> hits =
      FindMeshCrossings(mesh, Vec3d(10, 10, -5.0), Vec3d(0, 0, 10.0));
  EXPECT_TRUE(hits.empty());
}

}  // namespace
}  // namespace ns_lm

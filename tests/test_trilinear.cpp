#include <gtest/gtest.h>

#include "levelset3d_trilinear_measure/trilinear.hpp"

namespace ns_ls3tm {
namespace {

using ns_cg::Vec3d;

TEST(TrilinearTest, ValueAtEachCornerMatchesThatCornersInput) {
  const std::array<double, 8> v = {1, 2, 3, 4, 5, 6, 7, 8};
  for (int c = 0; c < 8; ++c) {
    const auto& o = kCubeCornerOffset[c];
    EXPECT_DOUBLE_EQ(TrilinearValue(v, o[0], o[1], o[2]), v[c])
        << "corner " << c;
  }
}

TEST(TrilinearTest, ValueAtCenterIsAverageOfAllCorners) {
  const std::array<double, 8> v = {1, 2, 3, 4, 5, 6, 7, 8};
  double expected = 0.0;
  for (double x : v) expected += x;
  expected /= 8.0;
  EXPECT_DOUBLE_EQ(TrilinearValue(v, 0.5, 0.5, 0.5), expected);
}

TEST(TrilinearTest, AxisAlignedLineCrossesOnceAtTheMidpoint) {
  // x=0 face negative, x=1 face positive: trilinear along any
  // fixed-(y,z) line is then an exact linear ramp from -1 to +1.
  const std::array<double, 8> v = {-1, 1, 1, -1, -1, 1, 1, -1};
  const std::vector<double> crossings =
      FindTrilinearCrossings(v, Vec3d(0.0, 0.3, 0.7), Vec3d(1.0, 0.0, 0.0));
  ASSERT_EQ(crossings.size(), 1u);
  EXPECT_NEAR(crossings[0], 0.5, 1e-6);
}

TEST(TrilinearTest, SaddleBelowThresholdGivesTwoCrossings) {
  // Case-5-style saddle (corners 0,2 inside), inside magnitude well
  // below the outside magnitude: the true trilinear surface pinches off
  // between the two inside corners.
  const std::array<double, 8> v = {-0.5, 1.0, -0.5, 1.0, 1.0, 1.0, 1.0, 1.0};
  const std::vector<double> crossings =
      FindTrilinearCrossings(v, Vec3d(0, 0, 0), Vec3d(1, 1, 0));
  EXPECT_EQ(crossings.size(), 2u);
}

TEST(TrilinearTest, SaddleAboveThresholdGivesNoCrossings) {
  // Same saddle, inside magnitude well above threshold: the two inside
  // corners are genuinely connected the whole way, per the asymptotic
  // decider (v0*v2 > v1*v3 here: 4 > 1).
  const std::array<double, 8> v = {-2.0, 1.0, -2.0, 1.0, 1.0, 1.0, 1.0, 1.0};
  const std::vector<double> crossings =
      FindTrilinearCrossings(v, Vec3d(0, 0, 0), Vec3d(1, 1, 0));
  EXPECT_TRUE(crossings.empty());
}

}  // namespace
}  // namespace ns_ls3tm

# levelset_metrology

A C++17 header-only library of measurement/comparison primitives for
level-set-derived geometry (`ns_cg::Grid2d`/`Grid3d`, `ns_cg::Polygon2d`,
`ns_cg::Mesh3d`) -- e.g. finding where a probe line crosses a shape's
boundary, and comparing that against the raw (bi/tri)linear interpolant
the boundary was extracted from. Intended to grow into a dimension-
agnostic metrology toolkit shared across
[`levelset2d_polygon`](../levelset2d_polygon) and
[`levelset3d_polygon`](../levelset3d_polygon), rather than being owned by
either.

Depends only on [`common_geometry`](../common_geometry) -- not on
`levelset2d_polygon` or `levelset3d_polygon` -- and operates purely on
already-extracted geometry (a `Mesh3d`, corner values, etc.) passed in by
the caller. It doesn't know how to run marching cubes, marching squares,
or any other extraction algorithm itself; that stays the extracting
project's job.

## Requirements

- CMake >= 3.20
- A C++17 compiler
- [Eigen3](https://eigen.tuxfamily.org/) (e.g. `brew install eigen` on macOS)
- A sibling checkout of [`common_geometry`](../common_geometry) at
  `../common_geometry` relative to this repository

## Building and testing

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
cd build && ctest --output-on-failure
```

## What's here

- `trilinear.hpp`: `TrilinearValue` (8 corner values -> value at any
  point) and `FindTrilinearCrossings` (every zero-crossing of that
  interpolant along an arbitrary probe line, via dense sampling +
  bisection -- works for any line direction, since the interpolant
  restricted to a line is a cubic in general, not just the linear case
  an axis-aligned probe degenerates to).
- `mesh_intersection.hpp`: `RayTriangleIntersect` (Moller-Trumbore) and
  `FindMeshCrossings` (every crossing of a probe line against an
  `ns_cg::Mesh3d`'s triangles).

Together these are what `levelset3d_polygon`'s
`analysis/saddle_intersection_analysis.cpp` uses to compare "where does
a marching-cubes mesh say the boundary is" against "where does the raw
trilinear interpolant actually cross zero" -- see that project's README
for the actual findings (a real, quantified topology disagreement at the
classic face-saddle ambiguity).

A 2D analog (`Polygon2d` edge crossings + bilinear interpolation) doesn't
exist yet; add it here, alongside these, whenever a 2D use case shows up.

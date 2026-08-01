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

[`docs/saddle_crossing_diff.html`](docs/saddle_crossing_diff.html)
([**view live**](https://k-naeba.github.io/levelset_metrology/saddle_crossing_diff.html))
visualizes that disagreement: crossing position vs. the inside-corner
magnitude `s` across the full sweep, plus two field heatmaps (`s =
0.30`, agreeing but offset; `s = 2.00`, disagreeing) showing why -- the
two negative lobes at the saddle's inside corners merge into one
connected region above the threshold, so the probe stops crossing zero
even though the fixed mesh triangulation still reports a gap.

A companion page,
[`docs/mesh_vs_trilinear_probe_height.html`](docs/mesh_vs_trilinear_probe_height.html)
([**view live**](https://k-naeba.github.io/levelset_metrology/mesh_vs_trilinear_probe_height.html)),
sweeps a different variable: corner values fixed, and the diagonal probe
line itself raised from the bottom face toward the top. Drag the height
slider to move the probe through an interactive 3D projection of the
actual `case5_mesh` triangles, and watch the sweep chart below it -- both
methods' crossings drift apart, then vanish together at the exact height
where the probe exits through the shared corner vertex. A second slider
exposes the inside-corner magnitude `s` itself: past `s = 1` a third
region opens up near the bottom face where the mesh reports a crossing
and the trilinear field reports none at all -- a genuine topological
disagreement, not just a positional one, with its own derived threshold
`z_lo = (s-1)/(1+s)`.

A 2D analog exists too, comparing `levelset2d_polygon`'s extracted-polygon
edge crossings against the raw bilinear field's zero crossings, within a
single grid cell.
[`docs/polygon_vs_bilinear_probe.html`](docs/polygon_vs_bilinear_probe.html)
([**view live**](https://k-naeba.github.io/levelset_metrology/polygon_vs_bilinear_probe.html))
visualizes it: drag a horizontal probe line across the cell and watch the
two methods' crossing positions pull apart, then a chart of that same
crossing position swept continuously across the full probe range. Unlike
the 3D case, `levelset2d_polygon`'s marching squares resolves the saddle
ambiguity correctly (via the true cell-center value), so here the two
methods only disagree on *where* the crossing is, never on *whether* one
exists.

All three pages are self-contained (no build step, no server) and served
directly from this repo's `docs/` folder via
[GitHub Pages](https://k-naeba.github.io/levelset_metrology/); they also
still work if you just open the file locally in a browser.

## Python bindings

A [pybind11](https://github.com/pybind/pybind11) module (`_levelset_metrology`,
re-exported as `levelset_metrology`) exposes this library's C++ API to
Python for interactive analysis in Jupyter, built via
[scikit-build-core](https://github.com/scikit-build/scikit-build-core).

Scoped exactly like the C++ library itself: `common_geometry` types
(`Mesh3d`) and this project's own functions
(`trilinear_value`, `find_trilinear_crossings`, `ray_triangle_intersect`,
`find_mesh_crossings`) only -- no extraction algorithm (no marching
cubes, no marching squares) is bound or depended on here. A small
pure-Python helper module, `known_cases`, fills the resulting gap for
demos: it hand-reconstructs the two classic marching-cubes face-saddle
triangulations (case 5 / case 10) directly from the corner-value
interpolation formula, without calling any extraction library. This is
verified (see `python/tests/test_bindings.py`) to reproduce, to within
floating-point tolerance, the exact crossing values that
`levelset3d_polygon`'s real marching-cubes implementation produces for
the same corner values.

### Install

```sh
uv venv --python 3.12 .venv
uv pip install --python .venv/bin/python -e ".[notebook]"
```

(Any Python >= 3.10 works; `uv venv --python 3.12` is just a known-good
pin for the notebook/Plotly/NumPy wheel stack at time of writing.)

### Usage

```python
import numpy as np
import levelset_metrology as lm

v = [-1.0, 1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0]
lm.find_trilinear_crossings(v, np.array([0.0, 0.3, 0.7]), np.array([1.0, 0.0, 0.0]))
# -> [0.5]
```

### Notebook

[`python/notebooks/exploration.ipynb`](python/notebooks/exploration.ipynb)
walks through the primitives above and reproduces
`levelset3d_polygon/analysis/saddle_intersection_analysis.cpp`'s case
5/10 sweep interactively (slider-driven, via `ipywidgets`), using
`known_cases.case5_mesh`/`case10_mesh` in place of a real marching-cubes
call. Launch with:

```sh
jupyter lab python/notebooks/
```

This Python layer used to live in a separate `levelset_python` repo
(alongside bindings for the other 4 projects); it was folded in here so
the metrology-specific Python surface has one home, decoupled from any
extraction algorithm's binding lifecycle.

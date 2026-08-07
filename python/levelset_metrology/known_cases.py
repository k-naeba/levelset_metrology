"""Hand-reconstructed meshes for a handful of classic marching-cubes
cases -- the single-corner case (case 1) and the two face-saddle cases
(case 5 and case 10) -- built directly from the corner-value
interpolation formula, not by calling any extraction library's
marching cubes implementation (this project deliberately doesn't
depend on one).

case5/case10 reproduce, exactly, the *fixed* two-triangle triangulation
the standard marching-cubes table always emits for these two cases,
independent of the corner values' magnitudes: two triangles, each
capping off one of the two "inside" corners (the well-known
disconnected disambiguation choice these bindings' saddle-comparison
demos work with -- see levelset3d_polygon's
analysis/saddle_intersection_analysis.cpp for the equivalent done via
the real marching cubes implementation instead). case1 is the
unambiguous single-corner baseline used to contrast against them: with
only one inside corner there's no disambiguation choice to make at
all, so it's the natural "what if there weren't a saddle" comparison.

Corner ordering and edge interpolation both match CUBE_CORNER_OFFSET's
convention exactly, so a mesh built here is directly comparable to one
built via any conforming marching-cubes implementation.
"""

from __future__ import annotations

import numpy as np

from . import Mesh3d

# Corner positions, matching CUBE_CORNER_OFFSET exactly.
_CORNERS = [
    np.array([0.0, 0.0, 0.0]), np.array([1.0, 0.0, 0.0]),
    np.array([1.0, 1.0, 0.0]), np.array([0.0, 1.0, 0.0]),
    np.array([0.0, 0.0, 1.0]), np.array([1.0, 0.0, 1.0]),
    np.array([1.0, 1.0, 1.0]), np.array([0.0, 1.0, 1.0]),
]


def _interpolate(v, a, b):
    """The zero-crossing point along edge (corner a, corner b)."""
    va, vb = v[a], v[b]
    t = va / (va - vb)
    return _CORNERS[a] + t * (_CORNERS[b] - _CORNERS[a])


def case1_mesh(v) -> Mesh3d:
    """corner 0 inside; corners 1-7 outside -- one solid region, no saddle.
    The single triangle capping corner 0 is exactly case5_mesh's first
    triangle (same three edges, same v0/v1/v3), so the two are directly
    comparable at matching s."""
    p_e0 = _interpolate(v, 0, 1)
    p_e3 = _interpolate(v, 3, 0)
    p_e8 = _interpolate(v, 0, 4)
    return Mesh3d([p_e0, p_e3, p_e8], [(0, 1, 2)])


def case5_mesh(v) -> Mesh3d:
    """corners 0, 2 inside; 1, 3 outside (top face 4-7 assumed outside,
    same as the saddle_intersection_analysis.cpp sweep)."""
    p_e0 = _interpolate(v, 0, 1)
    p_e3 = _interpolate(v, 3, 0)
    p_e8 = _interpolate(v, 0, 4)
    p_e1 = _interpolate(v, 1, 2)
    p_e10 = _interpolate(v, 2, 6)
    p_e2 = _interpolate(v, 2, 3)
    vertices = [p_e0, p_e3, p_e8, p_e1, p_e10, p_e2]
    triangles = [(0, 1, 2), (3, 4, 5)]
    return Mesh3d(vertices, triangles)


def case10_mesh(v) -> Mesh3d:
    """corners 1, 3 inside; 0, 2 outside -- the complementary saddle."""
    p_e1 = _interpolate(v, 1, 2)
    p_e0 = _interpolate(v, 0, 1)
    p_e9 = _interpolate(v, 1, 5)
    p_e2 = _interpolate(v, 2, 3)
    p_e11 = _interpolate(v, 3, 7)
    p_e3 = _interpolate(v, 3, 0)
    vertices = [p_e1, p_e0, p_e9, p_e2, p_e11, p_e3]
    triangles = [(0, 1, 2), (3, 4, 5)]
    return Mesh3d(vertices, triangles)

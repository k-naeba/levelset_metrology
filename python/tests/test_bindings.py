import numpy as np
import pytest

import levelset_metrology as lm


def test_trilinear_value_at_corners_matches_input():
    v = [1, 2, 3, 4, 5, 6, 7, 8]
    for c, (ox, oy, oz) in enumerate(lm.CUBE_CORNER_OFFSET):
        assert lm.trilinear_value(v, ox, oy, oz) == pytest.approx(v[c])


def test_axis_aligned_line_crosses_once_at_midpoint():
    v = [-1, 1, 1, -1, -1, 1, 1, -1]
    crossings = lm.find_trilinear_crossings(v, np.array([0.0, 0.3, 0.7]),
                                             np.array([1.0, 0.0, 0.0]))
    assert len(crossings) == 1
    assert crossings[0] == pytest.approx(0.5, abs=1e-6)


def test_ray_triangle_intersect_hit_and_miss():
    a, b, c = np.array([0.0, 0.0, 0.0]), np.array([2.0, 0.0, 0.0]), np.array([0.0, 2.0, 0.0])
    hit = lm.ray_triangle_intersect(np.array([0.5, 0.5, 5.0]), np.array([0.0, 0.0, -1.0]), a, b, c)
    assert hit == pytest.approx(5.0)

    miss = lm.ray_triangle_intersect(np.array([5.0, 5.0, 5.0]), np.array([0.0, 0.0, -1.0]), a, b, c)
    assert miss is None


def test_mesh3d_and_find_mesh_crossings():
    mesh = lm.Mesh3d(
        [np.array([0.0, 0.0, 0.0]), np.array([2.0, 0.0, 0.0]), np.array([0.0, 2.0, 0.0])],
        [(0, 1, 2)],
    )
    hits = lm.find_mesh_crossings(mesh, np.array([0.5, 0.5, -5.0]), np.array([0.0, 0.0, 10.0]))
    assert len(hits) == 1
    assert hits[0] == pytest.approx(0.5)


@pytest.mark.parametrize("s,expected", [(0.3, (0.1154, 0.8846)), (2.0, (1 / 3, 2 / 3))])
def test_known_case5_mesh_matches_cpp_reference(s, expected):
    # Reference values from levelset3d_polygon's
    # saddle_intersection_analysis.cpp (see that project's README).
    v = [-s, 1.0, -s, 1.0, 1.0, 1.0, 1.0, 1.0]
    origin, direction = np.array([0.0, 0.0, 0.0]), np.array([1.0, 1.0, 0.0])
    mesh = lm.known_cases.case5_mesh(v)
    ts = sorted(lm.find_mesh_crossings(mesh, origin, direction))
    assert len(ts) == 2
    assert ts[0] == pytest.approx(expected[0], abs=1e-3)
    assert ts[1] == pytest.approx(expected[1], abs=1e-3)


def test_known_case5_above_threshold_disagrees_with_trilinear():
    # s=2 is above the asymptotic-decider threshold (s=1): the mesh's
    # fixed two-triangle disconnection still reports a pinch-off, but
    # the true trilinear surface is connected (no crossings).
    v = [-2.0, 1.0, -2.0, 1.0, 1.0, 1.0, 1.0, 1.0]
    origin, direction = np.array([0.0, 0.0, 0.0]), np.array([1.0, 1.0, 0.0])
    mesh = lm.known_cases.case5_mesh(v)
    mesh_ts = lm.find_mesh_crossings(mesh, origin, direction)
    tri_ts = lm.find_trilinear_crossings(v, origin, direction)
    assert len(mesh_ts) == 2
    assert tri_ts == []


def test_plotting_helpers_produce_figures():
    mesh = lm.known_cases.case5_mesh([-2.0, 1.0, -2.0, 1.0, 1.0, 1.0, 1.0, 1.0])
    fig = lm.plotting.plot_mesh3d(mesh)
    assert len(fig.data) == 1

    fig2 = lm.plotting.plot_probe_comparison(
        mesh, np.array([0.0, 0.0, 0.0]), np.array([1.0, 1.0, 0.0]), [0.33, 0.67], [],
    )
    assert len(fig2.data) == 4

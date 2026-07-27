"""Plotly helpers for visualizing levelset_metrology results in Jupyter.

Scoped to what this project measures: a mesh, a probe line, and the
crossing points found along it (by either method). Nothing here knows
how the mesh was produced (marching cubes, hand-built, or otherwise).
"""

from __future__ import annotations

import numpy as np
import plotly.graph_objects as go


def mesh3d_trace(mesh, **kwargs):
    """A go.Mesh3d trace for a Mesh3d-like object (`.vertices`,
    `.triangles`)."""
    vertices = np.asarray(mesh.vertices)
    triangles = np.asarray(mesh.triangles)
    kwargs.setdefault("color", "#4488ff")
    kwargs.setdefault("opacity", 1.0)
    kwargs.setdefault("flatshading", True)
    kwargs.setdefault("lighting", dict(ambient=0.5, diffuse=0.8, specular=0.2))
    return go.Mesh3d(
        x=vertices[:, 0], y=vertices[:, 1], z=vertices[:, 2],
        i=triangles[:, 0], j=triangles[:, 1], k=triangles[:, 2],
        **kwargs,
    )


def plot_mesh3d(mesh, **kwargs) -> go.Figure:
    """A standalone figure showing one mesh, with an equal-aspect scene."""
    fig = go.Figure(data=[mesh3d_trace(mesh, **kwargs)])
    fig.update_scenes(aspectmode="data")
    return fig


def probe_line_trace(origin, direction, t_range=(0.0, 1.0), **kwargs):
    """A go.Scatter3d trace for the segment origin + t*direction, t in
    t_range."""
    origin = np.asarray(origin, dtype=float)
    direction = np.asarray(direction, dtype=float)
    p0 = origin + t_range[0] * direction
    p1 = origin + t_range[1] * direction
    kwargs.setdefault("mode", "lines")
    kwargs.setdefault("line", dict(color="black", width=4))
    kwargs.setdefault("name", "probe line")
    return go.Scatter3d(x=[p0[0], p1[0]], y=[p0[1], p1[1]], z=[p0[2], p1[2]], **kwargs)


def crossing_points_trace(origin, direction, ts, **kwargs):
    """A go.Scatter3d trace with a marker at origin + t*direction for
    each t in `ts` -- e.g. the output of find_mesh_crossings /
    find_trilinear_crossings."""
    origin = np.asarray(origin, dtype=float)
    direction = np.asarray(direction, dtype=float)
    points = np.array([origin + t * direction for t in ts]) if len(ts) else np.zeros((0, 3))
    kwargs.setdefault("mode", "markers")
    kwargs.setdefault("marker", dict(size=6, color="red"))
    kwargs.setdefault("name", "crossings")
    return go.Scatter3d(
        x=points[:, 0] if len(ts) else [],
        y=points[:, 1] if len(ts) else [],
        z=points[:, 2] if len(ts) else [],
        **kwargs,
    )


def plot_probe_comparison(mesh, origin, direction, mesh_ts, trilinear_ts,
                           t_range=(0.0, 1.0)) -> go.Figure:
    """A figure overlaying a mesh, its probe line, and both methods'
    crossing points (mesh crossings in red, trilinear crossings in
    green)."""
    fig = go.Figure(data=[
        mesh3d_trace(mesh, opacity=0.5),
        probe_line_trace(origin, direction, t_range),
        crossing_points_trace(origin, direction, mesh_ts,
                               name="mesh crossings",
                               marker=dict(size=7, color="red", symbol="circle")),
        crossing_points_trace(origin, direction, trilinear_ts,
                               name="trilinear crossings",
                               marker=dict(size=7, color="green", symbol="diamond")),
    ])
    fig.update_scenes(aspectmode="data")
    return fig

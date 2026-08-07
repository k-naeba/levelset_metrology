"""Python bindings for levelset3d_trilinear_measure: measurement primitives for
level-set-derived geometry (trilinear interpolation crossings,
mesh-triangle ray intersections).

Deliberately scoped to exactly what the C++ library itself provides --
no dependency on any extraction algorithm (levelset2d/3d_polygon,
rectilinear2d_boolean). Everything from the compiled extension is
re-exported at the top level, so ``import levelset3d_trilinear_measure as lm;
lm.Mesh3d(...)`` works directly.
"""

from ._levelset3d_trilinear_measure import *  # noqa: F401,F403
from ._levelset3d_trilinear_measure import __doc__ as _doc  # noqa: F401

from . import plotting  # noqa: F401
from . import known_cases  # noqa: F401 -- imported after Mesh3d is bound above

__all__ = [name for name in dir() if not name.startswith("_")]

// pybind11 bindings for levelset_metrology, scoped to exactly what this
// project itself provides: common_geometry's Vec3d (via pybind11/eigen.h,
// auto-converting to/from NumPy, no explicit binding needed) and Mesh3d
// (bound below, since FindMeshCrossings takes one), plus
// levelset_metrology's own functions. Deliberately does NOT depend on
// (or bind) any extraction-algorithm project (levelset2d/3d_polygon,
// rectilinear2d_boolean) -- matching this project's own C++ design,
// which only depends on common_geometry and operates on
// already-extracted geometry supplied by the caller.

#include <pybind11/eigen.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "common_geometry/mesh3d.hpp"
#include "levelset_metrology/levelset_metrology.hpp"

namespace py = pybind11;
using namespace ns_cg;

PYBIND11_MODULE(_levelset_metrology, m) {
  m.doc() =
      "Python bindings for levelset_metrology: measurement primitives for "
      "level-set-derived geometry (trilinear interpolation crossings, "
      "mesh-triangle ray intersections).";

  py::class_<Mesh3d>(m, "Mesh3d")
      .def(py::init<>())
      .def(py::init<std::vector<Vec3d>, std::vector<std::array<std::size_t, 3>>>(),
           py::arg("vertices"), py::arg("triangles"))
      .def_property(
          "vertices", [](Mesh3d& mesh) -> std::vector<Vec3d>& { return mesh.GetVertices(); },
          [](Mesh3d& mesh, std::vector<Vec3d> v) { mesh.GetVertices() = std::move(v); })
      .def_property(
          "triangles",
          [](Mesh3d& mesh) -> std::vector<std::array<std::size_t, 3>>& {
            return mesh.GetTriangles();
          },
          [](Mesh3d& mesh, std::vector<std::array<std::size_t, 3>> t) {
            mesh.GetTriangles() = std::move(t);
          });

  py::list corner_offsets;
  for (const auto& o : ns_lm::kCubeCornerOffset)
    corner_offsets.append(py::make_tuple(o[0], o[1], o[2]));
  m.attr("CUBE_CORNER_OFFSET") = corner_offsets;

  m.def("trilinear_value", &ns_lm::TrilinearValue, py::arg("v"), py::arg("x"), py::arg("y"),
        py::arg("z"), "v: the 8 cube-corner values, ordered per CUBE_CORNER_OFFSET.");

  m.def("find_trilinear_crossings", &ns_lm::FindTrilinearCrossings, py::arg("v"),
        py::arg("origin"), py::arg("dir"), py::arg("samples") = 4000,
        "Every t in [0,1] where the trilinear interpolant of `v` crosses "
        "zero along the line origin + t*dir.");

  m.def("ray_triangle_intersect", &ns_lm::RayTriangleIntersect, py::arg("origin"),
        py::arg("dir"), py::arg("a"), py::arg("b"), py::arg("c"),
        "The ray parameter t where segment [origin, origin+dir] crosses "
        "triangle (a,b,c), or None.");

  m.def("find_mesh_crossings", &ns_lm::FindMeshCrossings, py::arg("mesh"), py::arg("origin"),
        py::arg("dir"),
        "Every t in [0,1] where the segment origin + t*dir crosses one of "
        "mesh's triangles.");
}

cmake_minimum_required(VERSION 3.20)
project(LevelsetMetrology VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wpedantic)

# Sibling checkout dependency (relative path). Also brings in Eigen3 (found
# and linked PUBLIC by common_geometry's own CMakeLists.txt) transitively.
#
# Guarded: a consumer (e.g. levelset3d_polygon) that depends on both this
# project and common_geometry directly would otherwise add_subdirectory
# the same common_geometry path twice in one build tree, which CMake
# rejects. Whichever add_subdirectory call runs first wins; this one
# backs off if that's already happened.
if(NOT TARGET common_geometry)
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../common_geometry
                    ${CMAKE_CURRENT_BINARY_DIR}/common_geometry)
endif()

add_library(levelset_metrology INTERFACE)

target_include_directories(levelset_metrology
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(levelset_metrology INTERFACE cxx_std_17)

# INTERFACE (not PRIVATE): the public headers expose ns_cg::Vec3d/Mesh3d
# in public signatures, so consumers need common_geometry's (and
# transitively Eigen's) include paths too.
target_link_libraries(levelset_metrology INTERFACE common_geometry)

enable_testing()
add_subdirectory(tests)

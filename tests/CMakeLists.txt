include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)
FetchContent_MakeAvailable(googletest)

add_executable(test_trilinear test_trilinear.cpp)
add_executable(test_mesh_intersection test_mesh_intersection.cpp)
add_executable(test_levelset_metrology
    test_trilinear.cpp
    test_mesh_intersection.cpp
)

foreach(target test_trilinear test_mesh_intersection test_levelset_metrology)
    target_link_libraries(${target}
        PRIVATE
            levelset_metrology
            GTest::gtest_main
    )
    add_test(NAME ${target} COMMAND ${target})
endforeach()

add_library(task4_algo STATIC
    src/convex_hull.cpp
)

target_include_directories(task4_algo
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(task4_algo PUBLIC cxx_std_17)

add_library(compgeom::task4_algo ALIAS task4_algo)

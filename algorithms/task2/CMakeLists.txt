add_library(task2_algo STATIC
    src/segment_intersection.cpp
)

target_include_directories(task2_algo
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(task2_algo PUBLIC cxx_std_17)

add_library(compgeom::task2_algo ALIAS task2_algo)

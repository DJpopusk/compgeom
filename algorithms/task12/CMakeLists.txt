add_library(task12_algo STATIC
    src/point_locator.cpp
)

target_include_directories(task12_algo
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(task12_algo PUBLIC cxx_std_17)

add_library(compgeom::task12_algo ALIAS task12_algo)

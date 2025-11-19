add_library(task11_algo STATIC
    src/point_locator.cpp
)

target_include_directories(task11_algo
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(task11_algo PUBLIC cxx_std_17)

add_library(compgeom::task11_algo ALIAS task11_algo)

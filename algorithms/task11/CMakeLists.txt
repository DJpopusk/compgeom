add_library(task11_algo INTERFACE)

target_link_libraries(task11_algo
    INTERFACE
        compgeom::task10_algo
)

add_library(compgeom::task11_algo ALIAS task11_algo)

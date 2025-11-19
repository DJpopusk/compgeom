add_library(task10_algo STATIC
    src/polygon_boolean.cpp
)

if (NOT TARGET clipper2)
    add_library(clipper2 STATIC
        ${CMAKE_SOURCE_DIR}/third_party/clipper2/src/clipper.engine.cpp
        ${CMAKE_SOURCE_DIR}/third_party/clipper2/src/clipper.offset.cpp
        ${CMAKE_SOURCE_DIR}/third_party/clipper2/src/clipper.rectclip.cpp
    )
    target_include_directories(clipper2
        PUBLIC
            ${CMAKE_SOURCE_DIR}/third_party/clipper2
    )
endif()

target_include_directories(task10_algo
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/third_party/clipper2
)

target_link_libraries(task10_algo
    PUBLIC
        clipper2
)

add_library(compgeom::task10_algo ALIAS task10_algo)

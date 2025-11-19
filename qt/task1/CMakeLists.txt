set(TASK1_SOURCES
    main.cpp
    canvas_model.cpp
    canvas_widget.cpp
)

add_executable(task1_visual ${TASK1_SOURCES})

target_link_libraries(task1_visual
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        compgeom::task1_algo
)

target_include_directories(task1_visual
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(task1_visual PROPERTIES
    OUTPUT_NAME "task1_visual"
)

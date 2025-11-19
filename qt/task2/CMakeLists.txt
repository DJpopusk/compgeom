set(TASK2_SOURCES
    main.cpp
    main_window.cpp
    canvas_model.cpp
    canvas_widget.cpp
)

add_executable(task2_visual ${TASK2_SOURCES})

target_link_libraries(task2_visual
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        compgeom::task2_algo
)

target_include_directories(task2_visual
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(task2_visual PROPERTIES
    OUTPUT_NAME "task2_visual"
)

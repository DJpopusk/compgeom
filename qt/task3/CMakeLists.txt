set(TASK3_SOURCES
    main.cpp
    main_window.cpp
    canvas_model.cpp
    canvas_widget.cpp
)

add_executable(task3_visual ${TASK3_SOURCES})

target_link_libraries(task3_visual
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        compgeom::task3_algo
)

target_include_directories(task3_visual
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(task3_visual PROPERTIES
    OUTPUT_NAME "task3_visual"
)

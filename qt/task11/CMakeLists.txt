add_executable(task11_visual
    main.cpp
    canvas_model.cpp
    canvas_widget.cpp
    main_window.cpp
)

target_link_libraries(task11_visual
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        compgeom::task11_algo
)

set_target_properties(task11_visual PROPERTIES OUTPUT_NAME "task11_visual")

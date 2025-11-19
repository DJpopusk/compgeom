add_executable(task11_visual
    main.cpp
)

target_link_libraries(task11_visual
    PRIVATE
        polybool_common
)

set_target_properties(task11_visual PROPERTIES OUTPUT_NAME "task11_visual")

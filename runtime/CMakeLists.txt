add_executable(runtime
    src/main.cpp
    src/runtime_application.cpp
    src/runtime_application.h
    assets/shaders/triangle.vert
    assets/shaders/triangle.frag
)

target_include_directories(runtime PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(runtime PRIVATE engine glfw glad)

add_custom_command(TARGET runtime POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:runtime>/assets"
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES
    src/main.cpp
    src/runtime_application.cpp
    src/runtime_application.h
    assets/shaders/triangle.vert
    assets/shaders/triangle.frag
)

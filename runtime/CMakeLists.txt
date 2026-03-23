add_executable(runtime
    src/main.cpp
    src/application.cpp
)

target_include_directories(runtime PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(runtime PRIVATE engine glfw glad)

add_executable(runtime
    src/main.cpp
    src/runtime_application.cpp
    src/runtime_application.h
)

target_include_directories(runtime PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(runtime PRIVATE engine glfw glad)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES
    src/main.cpp
    src/runtime_application.cpp
    src/runtime_application.h
)

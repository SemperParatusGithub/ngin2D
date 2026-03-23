add_executable(editor
    src/main.cpp
    src/editor.cpp
)

target_include_directories(editor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(editor PRIVATE engine)

add_executable(editor
    src/main.cpp
    src/editor.cpp
    src/editor.h
)

target_include_directories(editor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(editor PRIVATE engine)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES
    src/main.cpp
    src/editor.cpp
    src/editor.h
)

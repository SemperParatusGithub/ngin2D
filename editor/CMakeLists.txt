find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGLWidgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(editor
    src/main.cpp
    src/new_project_dialog.cpp
    src/new_project_dialog.h
    src/project_launcher_window.cpp
    src/project_launcher_window.h
    src/editor.cpp
    src/editor.h
    src/file_dialog.cpp
    src/file_dialog.h
    src/qt_path_utils.h
    src/hierarchy_panel.cpp
    src/hierarchy_panel.h
    src/inspector_panel.cpp
    src/inspector_panel.h
    src/project_panel.cpp
    src/project_panel.h
    src/engine_viewport.cpp
    src/engine_viewport.h
    src/opengl_context_qt.cpp
    src/qt_input_adapter.cpp
    src/qt_input_adapter.h
)

target_include_directories(editor PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/engine/third_party/glad/include
)
# Engine is a normal linked library: consume it like third-party API (stable headers / ngin:: types).
target_link_libraries(editor PRIVATE Qt6::Widgets Qt6::OpenGLWidgets engine)

# Same layout as runtime: assets/shaders next to editor.exe so Renderer relative paths work.
set(_engine_shader_src "${CMAKE_SOURCE_DIR}/engine/assets/shaders")
set(_runtime_texture_src "${CMAKE_SOURCE_DIR}/runtime/assets/textures")
add_custom_command(
  TARGET editor POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:editor>/assets/shaders"
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${_engine_shader_src}" "$<TARGET_FILE_DIR:editor>/assets/shaders"
  COMMENT "Copy engine shaders next to editor.exe"
  VERBATIM
)
if(EXISTS "${_runtime_texture_src}")
  add_custom_command(
    TARGET editor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:editor>/assets/textures"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${_runtime_texture_src}" "$<TARGET_FILE_DIR:editor>/assets/textures"
    COMMENT "Copy runtime textures next to editor.exe"
    VERBATIM
  )
endif()

# Copy Qt DLLs next to editor.exe (otherwise Windows cannot find Qt6Widgets.dll, etc.)
if(WIN32)
  find_program(WINDEPLOYQT_EXECUTABLE windeployqt
    HINTS "${Qt6_DIR}/../../../bin" "$ENV{QTDIR}/bin"
    REQUIRED
  )
  add_custom_command(
    TARGET editor POST_BUILD
    COMMAND "${WINDEPLOYQT_EXECUTABLE}"
      "$<IF:$<CONFIG:Debug>,--debug,--release>"
      "$<TARGET_FILE:editor>"
    WORKING_DIRECTORY "$<TARGET_FILE_DIR:editor>"
    COMMENT "windeployqt: Qt runtime next to editor"
  )
endif()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES
    src/main.cpp
    src/new_project_dialog.cpp
    src/new_project_dialog.h
    src/project_launcher_window.cpp
    src/project_launcher_window.h
    src/editor.cpp
    src/editor.h
    src/file_dialog.cpp
    src/file_dialog.h
    src/qt_path_utils.h
    src/hierarchy_panel.cpp
    src/hierarchy_panel.h
    src/inspector_panel.cpp
    src/inspector_panel.h
    src/project_panel.cpp
    src/project_panel.h
    src/engine_viewport.cpp
    src/engine_viewport.h
    src/opengl_context_qt.cpp
    src/qt_input_adapter.cpp
    src/qt_input_adapter.h
)

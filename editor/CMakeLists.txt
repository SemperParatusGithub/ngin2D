find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(editor
    src/main.cpp
    src/editor.cpp
    src/editor.h
    src/dashboard_theme.cpp
    src/dashboard_theme.h
)

target_include_directories(editor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(editor PRIVATE Qt6::Widgets engine)

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
    src/editor.cpp
    src/editor.h
    src/dashboard_theme.cpp
    src/dashboard_theme.h
)

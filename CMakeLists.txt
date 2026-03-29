cmake_minimum_required(VERSION 3.25)
project(ngin2D C CXX)

# Qt: use one of:
#   cmake -DQT6_PREFIX_PATH=C:/Qt/<ver>/<kit> ...
#   cmake -DCMAKE_PREFIX_PATH=C:/Qt/<ver>/<kit> ...
#   set env QTDIR to that same kit directory (contains lib/cmake/Qt6).
set(QT6_PREFIX_PATH "" CACHE PATH "Path to Qt 6 kit (e.g. C:/Qt/6.8.0/msvc2022_64)")
if(QT6_PREFIX_PATH)
  list(PREPEND CMAKE_PREFIX_PATH "${QT6_PREFIX_PATH}")
endif()
if(DEFINED ENV{QTDIR} AND NOT "$ENV{QTDIR}" STREQUAL "")
  list(PREPEND CMAKE_PREFIX_PATH "$ENV{QTDIR}")
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(engine)
add_subdirectory(editor)
add_subdirectory(runtime)

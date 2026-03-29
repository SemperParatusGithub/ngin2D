# ngin2D

[![Build (Windows)](https://github.com/SemperParatusGithub/ngin2D/actions/workflows/build-windows.yml/badge.svg?branch=main)](https://github.com/SemperParatusGithub/ngin2D/actions/workflows/build-windows.yml)
[![Build (Linux)](https://github.com/SemperParatusGithub/ngin2D/actions/workflows/build-linux.yml/badge.svg?branch=main)](https://github.com/SemperParatusGithub/ngin2D/actions/workflows/build-linux.yml)
[![Build (macOS)](https://github.com/SemperParatusGithub/ngin2D/actions/workflows/build-macos.yml/badge.svg?branch=main)](https://github.com/SemperParatusGithub/ngin2D/actions/workflows/build-macos.yml)

A 2D game engine written in C++23.

## Structure

| Directory  | Type            | Description                          |
|------------|-----------------|--------------------------------------|
| `engine/`  | Static library  | Core engine — shared by editor and runtime |
| `editor/`  | Executable      | Editor application                   |
| `runtime/` | Executable      | Standalone game runtime              |

## Building

The **editor** target links Qt 6. On Windows, CMake must be able to find your Qt kit (the directory that contains `lib/cmake/Qt6`). Use one of:

- **`-DQT6_PREFIX_PATH`** — e.g. `-DQT6_PREFIX_PATH="C:/Qt/6.11.0/msvc2022_64"` (adjust version and kit to match your install)
- **`CMAKE_PREFIX_PATH`** — same path as above
- **`QTDIR`** environment variable — set to the same kit directory

Install the **MSVC** desktop components for your Qt version (Qt Maintenance Tool → add components).

```bash
cmake -B build -DQT6_PREFIX_PATH="C:/Qt/<version>/msvc2022_64"
cmake --build build
```

### Visual Studio (Windows)

Generate a Visual Studio solution (example: Visual Studio 2026, x64, Qt 6.11 MSVC kit — change generator, `-A`, and `QT6_PREFIX_PATH` to match your machine):

```bash
cmake -B build -G "Visual Studio 18 2026" -A x64 -S . -DQT6_PREFIX_PATH="C:/Qt/6.11.0/msvc2022_64"
```

Open `build/ngin2D.sln` in Visual Studio. Build from the IDE, or from the command line:

```bash
cmake --build build --config Release
```

If you switch Visual Studio generators, delete `build/CMakeCache.txt` and `build/CMakeFiles` (or use a fresh build directory) before re-running CMake.

Requires CMake 4.2+ and a C++23-capable compiler.

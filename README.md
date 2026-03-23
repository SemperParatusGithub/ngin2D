# ngin2D

A 2D game engine written in C++23.

## Structure

| Directory  | Type            | Description                          |
|------------|-----------------|--------------------------------------|
| `engine/`  | Static library  | Core engine — shared by editor and runtime |
| `editor/`  | Executable      | Editor application                   |
| `runtime/` | Executable      | Standalone game runtime              |

## Building

```bash
cmake -B build
cmake --build build
```

### Visual Studio 2026

```bash
cmake -B build -G "Visual Studio 18 2026" -A x64
```
```bash
cmake --build build --config Release
```

Requires CMake 4.2+ and a C++23-capable compiler.

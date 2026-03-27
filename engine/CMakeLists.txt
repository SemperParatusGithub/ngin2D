set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(third_party/glfw)

set(SPDLOG_BUILD_EXAMPLE OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_BENCH OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(SPDLOG_FMT_EXTERNAL OFF CACHE BOOL "" FORCE)

add_subdirectory(third_party/spdlog)
add_subdirectory(third_party/glm)

add_library(glad STATIC
    third_party/glad/src/glad.c
    third_party/glad/include/glad/glad.h
    third_party/glad/include/KHR/khrplatform.h
)

target_include_directories(glad PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glad/include
)

add_library(engine STATIC
    src/application.h
    src/application.cpp
    src/event.h
    src/engine.cpp
    src/engine.h
    src/platform_detection.h
    src/window.h
    src/window.cpp
    src/opengl_context.h
    src/opengl_context.cpp
    src/graphics/shader.h
    src/graphics/shader.cpp
    src/graphics/texture.h
    src/graphics/texture.cpp
    src/graphics/vertex_buffer.h
    src/graphics/vertex_buffer.cpp
    src/graphics/index_buffer.h
    src/graphics/index_buffer.cpp
    src/graphics/graphics_pipeline.h
    src/graphics/graphics_pipeline.cpp
    src/Transform.h
    src/Transform.cpp
    src/camera.h
    src/camera.cpp
    src/key_codes.h
    src/input.h
    src/input.cpp
    src/core/build_config.h
    src/core/types.h
    src/core/log.h
    src/core/log.cpp
    src/core/assert.h
    src/core/core.h
)

target_include_directories(engine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(engine PRIVATE glfw glad spdlog::spdlog)
target_link_libraries(engine PUBLIC glm::glm)
target_compile_definitions(engine PUBLIC GLM_ENABLE_EXPERIMENTAL)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES
    src/application.h
    src/application.cpp
    src/event.h
    src/engine.cpp
    src/engine.h
    src/platform_detection.h
    src/window.h
    src/window.cpp
    src/opengl_context.h
    src/opengl_context.cpp
    src/graphics/shader.h
    src/graphics/shader.cpp
    src/graphics/texture.h
    src/graphics/texture.cpp
    src/graphics/vertex_buffer.h
    src/graphics/vertex_buffer.cpp
    src/graphics/index_buffer.h
    src/graphics/index_buffer.cpp
    src/graphics/graphics_pipeline.h
    src/graphics/graphics_pipeline.cpp
    src/Transform.h
    src/Transform.cpp
    src/camera.h
    src/camera.cpp
    src/key_codes.h
    src/input.h
    src/input.cpp
    src/core/build_config.h
    src/core/types.h
    src/core/log.h
    src/core/log.cpp
    src/core/assert.h
    src/core/core.h
    third_party/glad/src/glad.c
    third_party/glad/include/glad/glad.h
    third_party/glad/include/KHR/khrplatform.h
)

include(FetchContent)

set(GLFW_FETCH_VERSION "3.4" CACHE STRING "GLFW release tag or commit for FetchContent")
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG ${GLFW_FETCH_VERSION}
    GIT_SHALLOW TRUE
)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw)

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
    src/event.h
    src/event_queue.h
    src/event_queue.cpp
    src/scene/scene.h
    src/scene/scene.cpp
    src/scene/entity.h
    src/scene/entity.cpp
    src/scene/components/components.h
    src/scene/components/id_component.h
    src/scene/components/tag_component.h
    src/scene/components/sprite_component.h
    src/scene/components/transform_component.h
    src/engine.cpp
    src/engine.h
    src/window.h
    src/window.cpp
    src/graphics/opengl_context.h
    src/graphics/opengl_context.cpp
    src/graphics/shader.h
    src/graphics/shader.cpp
    src/graphics/texture.h
    src/graphics/texture.cpp
    src/graphics/sprite.h
    src/graphics/sprite.cpp
    src/graphics/renderer.h
    src/graphics/renderer.cpp
    src/graphics/vertex_buffer.h
    src/graphics/vertex_buffer.cpp
    src/graphics/index_buffer.h
    src/graphics/index_buffer.cpp
    src/graphics/framebuffer.h
    src/graphics/framebuffer.cpp
    src/graphics/graphics_pipeline.h
    src/graphics/graphics_pipeline.cpp
    src/graphics/camera.h
    src/graphics/camera.cpp
    src/transform.h
    src/transform.cpp
    src/key_codes.h
    src/input.h
    src/input.cpp
    src/core/build_config.h
    src/core/platform_detection.h
    src/core/types.h
    src/core/log.h
    src/core/log.cpp
    src/core/assert.h
    src/core/core.h
)

target_include_directories(engine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/entt/src
)

target_link_libraries(engine PRIVATE glfw glad spdlog::spdlog)
target_link_libraries(engine PUBLIC glm::glm)
target_compile_definitions(engine PUBLIC GLM_ENABLE_EXPERIMENTAL)

# GLSL sources live under engine/assets/shaders (not compiled; listed for IDE visibility).
set(ENGINE_SHADER_ASSETS
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders/triangle.vert
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders/triangle.frag
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders/quad.vert
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders/quad.frag
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders/circle.vert
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders/circle.frag
)
target_sources(engine PRIVATE ${ENGINE_SHADER_ASSETS})
set_source_files_properties(${ENGINE_SHADER_ASSETS} PROPERTIES HEADER_FILE_ONLY TRUE)
source_group("assets/shaders" FILES ${ENGINE_SHADER_ASSETS})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES
    src/event.h
    src/event_queue.h
    src/event_queue.cpp
    src/scene/scene.h
    src/scene/scene.cpp
    src/scene/entity.h
    src/scene/entity.cpp
    src/scene/components/components.h
    src/scene/components/id_component.h
    src/scene/components/tag_component.h
    src/scene/components/sprite_component.h
    src/scene/components/transform_component.h
    src/engine.cpp
    src/engine.h
    src/window.h
    src/window.cpp
    src/graphics/opengl_context.h
    src/graphics/opengl_context.cpp
    src/graphics/shader.h
    src/graphics/shader.cpp
    src/graphics/texture.h
    src/graphics/texture.cpp
    src/graphics/sprite.h
    src/graphics/sprite.cpp
    src/graphics/renderer.h
    src/graphics/renderer.cpp
    src/graphics/vertex_buffer.h
    src/graphics/vertex_buffer.cpp
    src/graphics/index_buffer.h
    src/graphics/index_buffer.cpp
    src/graphics/framebuffer.h
    src/graphics/framebuffer.cpp
    src/graphics/graphics_pipeline.h
    src/graphics/graphics_pipeline.cpp
    src/graphics/camera.h
    src/graphics/camera.cpp
    src/transform.h
    src/transform.cpp
    src/key_codes.h
    src/input.h
    src/input.cpp
    src/core/build_config.h
    src/core/platform_detection.h
    src/core/types.h
    src/core/log.h
    src/core/log.cpp
    src/core/assert.h
    src/core/core.h
    third_party/glad/src/glad.c
    third_party/glad/include/glad/glad.h
    third_party/glad/include/KHR/khrplatform.h
)

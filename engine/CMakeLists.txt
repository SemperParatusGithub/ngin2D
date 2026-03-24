set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(third_party/glfw)

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
    src/engine.cpp
    src/engine.h
    src/platform_detection.h
    src/core/build_config.h
    src/core/types.h
    src/core/assert.h
    src/core/core.h
)

target_include_directories(engine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(engine PRIVATE glfw glad)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES
    src/application.h
    src/application.cpp
    src/engine.cpp
    src/engine.h
    src/platform_detection.h
    src/core/build_config.h
    src/core/types.h
    src/core/assert.h
    src/core/core.h
    third_party/glad/src/glad.c
    third_party/glad/include/glad/glad.h
    third_party/glad/include/KHR/khrplatform.h
)

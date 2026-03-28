add_executable(runtime
    src/main.cpp
    src/runtime_application.cpp
    src/runtime_application.h
)

target_include_directories(runtime PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(runtime PRIVATE engine glfw glad)

# Single output directory next to copied assets (all MSVC configs).
set(_runtime_exe_dir "${CMAKE_CURRENT_BINARY_DIR}")
if(CMAKE_CONFIGURATION_TYPES)
    foreach(_cfg ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER ${_cfg} _cfg_u)
        set_target_properties(runtime PROPERTIES "RUNTIME_OUTPUT_DIRECTORY_${_cfg_u}" "${_runtime_exe_dir}")
    endforeach()
else()
    set_target_properties(runtime PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${_runtime_exe_dir}")
endif()

set_target_properties(runtime PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${_runtime_exe_dir}")

# Shaders: canonical tree under engine/assets/shaders.
# Textures: canonical tree under runtime/assets/textures.
# Copy both next to runtime.exe so paths like assets/shaders/quad.vert work at runtime.
set(_engine_shader_src "${CMAKE_SOURCE_DIR}/engine/assets/shaders")
set(_runtime_texture_src "${CMAKE_CURRENT_SOURCE_DIR}/assets/textures")

add_custom_command(
    TARGET runtime POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:runtime>/assets/shaders"
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:runtime>/assets/textures"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${_engine_shader_src}" "$<TARGET_FILE_DIR:runtime>/assets/shaders"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${_runtime_texture_src}" "$<TARGET_FILE_DIR:runtime>/assets/textures"
    COMMENT "Copy engine shaders and runtime textures next to runtime.exe"
    VERBATIM
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES
    src/main.cpp
    src/runtime_application.cpp
    src/runtime_application.h
)

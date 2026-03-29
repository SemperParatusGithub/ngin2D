#include "graphics/opengl_context.h"

#include "core/build_config.h"
#include "core/log.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace ngin {
namespace {

#if NGIN_DEBUG
#ifndef GL_DEBUG_OUTPUT
    #define GL_DEBUG_OUTPUT 0x92E0
#endif
#ifndef GL_DEBUG_OUTPUT_SYNCHRONOUS
    #define GL_DEBUG_OUTPUT_SYNCHRONOUS 0x8242
#endif
#ifndef GL_DEBUG_SEVERITY_HIGH
    #define GL_DEBUG_SEVERITY_HIGH 0x9146
#endif
#ifndef GL_DEBUG_SEVERITY_MEDIUM
    #define GL_DEBUG_SEVERITY_MEDIUM 0x9147
#endif
#ifndef GL_DEBUG_SEVERITY_LOW
    #define GL_DEBUG_SEVERITY_LOW 0x9148
#endif
#ifndef GL_DEBUG_SEVERITY_NOTIFICATION
    #define GL_DEBUG_SEVERITY_NOTIFICATION 0x826B
#endif
#ifndef GL_CONTEXT_FLAGS
    #define GL_CONTEXT_FLAGS 0x821E
#endif
#ifndef GL_CONTEXT_FLAG_DEBUG_BIT
    #define GL_CONTEXT_FLAG_DEBUG_BIT 0x00000002
#endif

using GLDebugCallback = void (APIENTRY*)(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar*, const void*);
using GLDebugMessageCallbackProc = void (APIENTRY*)(GLDebugCallback callback, const void* user_param);
using GLDebugMessageControlProc =
    void (APIENTRY*)(GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint* ids, GLboolean enabled);

void APIENTRY opengl_debug_callback(
    GLenum,
    GLenum,
    GLuint,
    GLenum severity,
    GLsizei,
    const GLchar* message,
    const void*
) {
    const char* text = message != nullptr ? message : "OpenGL debug callback message is null";

    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            NGIN_ERROR("[OpenGL] {}", text);
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            NGIN_WARN("[OpenGL] {}", text);
            break;
        case GL_DEBUG_SEVERITY_LOW:
            NGIN_INFO("[OpenGL] {}", text);
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
        default:
            NGIN_TRACE("[OpenGL] {}", text);
            break;
    }
}

void setup_opengl_debug_output() {
    GLint context_flags = 0;
    glGetIntegerv(GL_CONTEXT_FLAGS, &context_flags);
    if ((context_flags & GL_CONTEXT_FLAG_DEBUG_BIT) == 0) {
        NGIN_WARN("OpenGL debug context flag is not set. Debug callbacks may be unavailable.");
    }

    const auto debug_message_callback =
        reinterpret_cast<GLDebugMessageCallbackProc>(glfwGetProcAddress("glDebugMessageCallback"));
    const auto debug_message_control =
        reinterpret_cast<GLDebugMessageControlProc>(glfwGetProcAddress("glDebugMessageControl"));

    if (!debug_message_callback || !debug_message_control) {
        NGIN_WARN("OpenGL debug callback functions are not available on this driver/context.");
        return;
    }

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    debug_message_callback(opengl_debug_callback, nullptr);

    NGIN_INFO("OpenGL debug callback enabled.");
}
#endif

} // namespace

OpenGLContext::OpenGLContext(GLFWwindow* native_window_handle) : m_window_handle(native_window_handle) {}

bool OpenGLContext::init() {
    if (!m_window_handle) {
        NGIN_ERROR("OpenGLContext init failed: native window handle is null.");
        return false;
    }

    glfwMakeContextCurrent(m_window_handle);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        NGIN_ERROR("Failed to initialize GLAD");
        return false;
    }

#if NGIN_DEBUG
    setup_opengl_debug_output();
#endif

    int framebuffer_width = 0;
    int framebuffer_height = 0;
    glfwGetFramebufferSize(m_window_handle, &framebuffer_width, &framebuffer_height);
    glViewport(0, 0, framebuffer_width, framebuffer_height);
    return true;
}

void OpenGLContext::set_vsync(bool enabled) {
    glfwSwapInterval(enabled ? 1 : 0);
}

} // namespace ngin

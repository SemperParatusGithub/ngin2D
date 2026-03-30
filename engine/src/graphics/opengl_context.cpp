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

void setup_opengl_debug_output(GladLoadProc loader) {
    GLint context_flags = 0;
    glGetIntegerv(GL_CONTEXT_FLAGS, &context_flags);
    if ((context_flags & GL_CONTEXT_FLAG_DEBUG_BIT) == 0) {
        NGIN_WARN("OpenGL debug context flag is not set. Debug callbacks may be unavailable.");
    }

    const auto debug_message_callback =
        reinterpret_cast<GLDebugMessageCallbackProc>(loader("glDebugMessageCallback"));
    const auto debug_message_control =
        reinterpret_cast<GLDebugMessageControlProc>(loader("glDebugMessageControl"));

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

bool glad_init_with_loader_impl(GladLoadProc get_proc_address, bool set_viewport_from_context) {
    if (!get_proc_address) {
        NGIN_ERROR("OpenGLContext: null GL loader.");
        return false;
    }

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(get_proc_address))) {
        NGIN_ERROR("Failed to initialize GLAD");
        return false;
    }

#if NGIN_DEBUG
    setup_opengl_debug_output(get_proc_address);
#endif

    if (set_viewport_from_context) {
        GLint viewport[4] = {0, 0, 0, 0};
        glGetIntegerv(GL_VIEWPORT, viewport);
        if (viewport[2] > 0 && viewport[3] > 0) {
            glViewport(0, 0, viewport[2], viewport[3]);
        }
    }

    return true;
}

} // namespace

bool OpenGLContext::create_from_glfw(GLFWwindow* native_window_handle) {
    if (!native_window_handle) {
        NGIN_ERROR("OpenGLContext::create_from_glfw failed: native window handle is null.");
        return false;
    }

    m_window_handle = native_window_handle;
    m_is_external = false;
    glfwMakeContextCurrent(m_window_handle);

    if (!glad_init_with_loader_impl(reinterpret_cast<GladLoadProc>(glfwGetProcAddress), true)) {
        return false;
    }

    int framebuffer_width = 0;
    int framebuffer_height = 0;
    glfwGetFramebufferSize(m_window_handle, &framebuffer_width, &framebuffer_height);
    glViewport(0, 0, framebuffer_width, framebuffer_height);
    return true;
}

bool OpenGLContext::init_external(GladLoadProc get_proc_address, bool set_viewport_from_context) {
    if (!m_is_external) {
        NGIN_ERROR("OpenGLContext::init_external() is only for external (non-GLFW) contexts.");
        return false;
    }
    return glad_init_with_loader_impl(get_proc_address, set_viewport_from_context);
}

void OpenGLContext::set_vsync(bool enabled) {
    if (m_is_external) {
        // Vsync for Qt is set via QSurfaceFormat::setSwapInterval on the application or surface.
        (void)enabled;
        return;
    }
    glfwSwapInterval(enabled ? 1 : 0);
}

} // namespace ngin

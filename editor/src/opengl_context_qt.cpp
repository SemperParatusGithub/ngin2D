#include "graphics/opengl_context.h"

#include "core/log.h"

#include <QByteArray>
#include <QOpenGLContext>
#include <QOpenGLWidget>

namespace ngin {

namespace {

void* qt_gl_loader(const char* name) {
    QOpenGLContext* ctx = QOpenGLContext::currentContext();
    if (!ctx || !name) {
        return nullptr;
    }
    return reinterpret_cast<void*>(ctx->getProcAddress(QByteArray(name)));
}

} // namespace

bool OpenGLContext::create_from_qt(QOpenGLWidget* surface) {
    if (!surface) {
        NGIN_ERROR("OpenGLContext::create_from_qt: null surface");
        return false;
    }

    surface->makeCurrent();
    if (!QOpenGLContext::currentContext()) {
        NGIN_ERROR("OpenGLContext::create_from_qt: no current GL context after makeCurrent");
        return false;
    }

    m_window_handle = nullptr;
    m_is_external = true;
    return init_external(qt_gl_loader, true);
}

} // namespace ngin

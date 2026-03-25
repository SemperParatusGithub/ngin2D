#include "graphics/vertex_buffer.h"

#include <glad/glad.h>

namespace ngin {

VertexBuffer::VertexBuffer(const void* data, u32 size) : m_renderer_id(0) {
    glGenBuffers(1, &m_renderer_id);
    glBindBuffer(GL_ARRAY_BUFFER, m_renderer_id);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(size), data, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer() {
    if (m_renderer_id != 0) {
        glDeleteBuffers(1, &m_renderer_id);
        m_renderer_id = 0;
    }
}

VertexBuffer::VertexBuffer(VertexBuffer&& other) noexcept : m_renderer_id(other.m_renderer_id) {
    other.m_renderer_id = 0;
}

VertexBuffer& VertexBuffer::operator=(VertexBuffer&& other) noexcept {
    if (this != &other) {
        if (m_renderer_id != 0) {
            glDeleteBuffers(1, &m_renderer_id);
        }
        m_renderer_id = other.m_renderer_id;
        other.m_renderer_id = 0;
    }
    return *this;
}

void VertexBuffer::bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, m_renderer_id);
}

void VertexBuffer::unbind() const {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

} // namespace ngin

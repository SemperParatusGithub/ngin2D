#include "graphics/index_buffer.h"

#include <glad/glad.h>

namespace ngin {

IndexBuffer::IndexBuffer(const u32* indices, u32 count) : m_renderer_id(0), m_count(count) {
    glGenBuffers(1, &m_renderer_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_renderer_id);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(count * static_cast<u32>(sizeof(u32))),
        indices,
        GL_STATIC_DRAW
    );
}

IndexBuffer::~IndexBuffer() {
    if (m_renderer_id != 0) {
        glDeleteBuffers(1, &m_renderer_id);
        m_renderer_id = 0;
    }
}

IndexBuffer::IndexBuffer(IndexBuffer&& other) noexcept
    : m_renderer_id(other.m_renderer_id), m_count(other.m_count) {
    other.m_renderer_id = 0;
    other.m_count = 0;
}

IndexBuffer& IndexBuffer::operator=(IndexBuffer&& other) noexcept {
    if (this != &other) {
        if (m_renderer_id != 0) {
            glDeleteBuffers(1, &m_renderer_id);
        }
        m_renderer_id = other.m_renderer_id;
        m_count = other.m_count;
        other.m_renderer_id = 0;
        other.m_count = 0;
    }
    return *this;
}

void IndexBuffer::bind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_renderer_id);
}

void IndexBuffer::unbind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

} // namespace ngin

#include "graphics/shader.h"

#include "core/log.h"

#include <glad/glad.h>

namespace {

GLuint compile_shader(GLenum type, std::string_view source) {
    const GLuint shader = glCreateShader(type);
    const GLchar* source_ptr = source.data();
    const GLint source_len = static_cast<GLint>(source.size());
    glShaderSource(shader, 1, &source_ptr, &source_len);
    glCompileShader(shader);

    GLint success = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE) {
        char info_log[512] = {};
        glGetShaderInfoLog(shader, sizeof(info_log), nullptr, info_log);
        NGIN_ERROR("Shader compile failed: {}", info_log);
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

} // namespace

namespace ngin {

Shader::Shader(std::string_view vertex_source, std::string_view fragment_source) : m_renderer_id(0) {
    const GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_source);
    const GLuint fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_source);
    if (vertex_shader == 0 || fragment_shader == 0) {
        if (vertex_shader != 0) {
            glDeleteShader(vertex_shader);
        }
        if (fragment_shader != 0) {
            glDeleteShader(fragment_shader);
        }
        return;
    }

    m_renderer_id = glCreateProgram();
    glAttachShader(m_renderer_id, vertex_shader);
    glAttachShader(m_renderer_id, fragment_shader);
    glLinkProgram(m_renderer_id);

    GLint success = GL_FALSE;
    glGetProgramiv(m_renderer_id, GL_LINK_STATUS, &success);
    if (success != GL_TRUE) {
        char info_log[512] = {};
        glGetProgramInfoLog(m_renderer_id, sizeof(info_log), nullptr, info_log);
        NGIN_ERROR("Shader link failed: {}", info_log);
        glDeleteProgram(m_renderer_id);
        m_renderer_id = 0;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

Shader::~Shader() {
    if (m_renderer_id != 0) {
        glDeleteProgram(m_renderer_id);
        m_renderer_id = 0;
    }
}

Shader::Shader(Shader&& other) noexcept : m_renderer_id(other.m_renderer_id) {
    other.m_renderer_id = 0;
}

Shader& Shader::operator=(Shader&& other) noexcept {
    if (this != &other) {
        if (m_renderer_id != 0) {
            glDeleteProgram(m_renderer_id);
        }
        m_renderer_id = other.m_renderer_id;
        other.m_renderer_id = 0;
    }
    return *this;
}

void Shader::bind() const {
    glUseProgram(m_renderer_id);
}

void Shader::unbind() const {
    glUseProgram(0);
}

} // namespace ngin

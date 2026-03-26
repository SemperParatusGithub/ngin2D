#include "graphics/shader.h"

#include "core/log.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <string>

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

GLuint create_program(std::string_view vertex_source, std::string_view fragment_source) {
    const GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_source);
    const GLuint fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_source);
    if (vertex_shader == 0 || fragment_shader == 0) {
        if (vertex_shader != 0) {
            glDeleteShader(vertex_shader);
        }
        if (fragment_shader != 0) {
            glDeleteShader(fragment_shader);
        }
        return 0;
    }

    const GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    GLint success = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success != GL_TRUE) {
        char info_log[512] = {};
        glGetProgramInfoLog(program, sizeof(info_log), nullptr, info_log);
        NGIN_ERROR("Shader link failed: {}", info_log);
        glDeleteProgram(program);
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        return 0;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    return program;
}

bool read_text_file(const std::filesystem::path& path, std::string& out_source) {
    std::ifstream file(path, std::ios::in);
    if (!file.is_open()) {
        NGIN_ERROR("Failed to open shader file: {}", path.string());
        return false;
    }

    out_source.assign(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    );
    return true;
}

} // namespace

namespace ngin {

Shader::Shader() : m_renderer_id(0) {}

Shader::Shader(std::string_view vertex_source, std::string_view fragment_source) : m_renderer_id(0) {
    load_from_source(vertex_source, fragment_source);
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

bool Shader::load_from_source(std::string_view vertex_source, std::string_view fragment_source) {
    const GLuint new_program = create_program(vertex_source, fragment_source);
    if (new_program == 0) {
        return false;
    }

    if (m_renderer_id != 0) {
        glDeleteProgram(m_renderer_id);
    }
    m_renderer_id = new_program;
    return true;
}

bool Shader::load_from_files(
    const std::filesystem::path& vertex_path,
    const std::filesystem::path& fragment_path
) {
    std::string vertex_source;
    std::string fragment_source;
    if (!read_text_file(vertex_path, vertex_source)) {
        return false;
    }
    if (!read_text_file(fragment_path, fragment_source)) {
        return false;
    }
    return load_from_source(vertex_source, fragment_source);
}

i32 Shader::get_uniform_location(std::string_view name) const {
    const std::string uniform_name(name);
    const GLint location = glGetUniformLocation(m_renderer_id, uniform_name.c_str());
    if (location < 0) {
        NGIN_WARN("Uniform '{}' not found in shader program {}", uniform_name, m_renderer_id);
    }
    return location;
}

void Shader::set_uniform_i32(std::string_view name, i32 value) const {
    bind();
    const GLint location = get_uniform_location(name);
    if (location >= 0) {
        glUniform1i(location, value);
    }
}

void Shader::set_uniform_f32(std::string_view name, f32 value) const {
    bind();
    const GLint location = get_uniform_location(name);
    if (location >= 0) {
        glUniform1f(location, value);
    }
}

void Shader::set_uniform_vec2(std::string_view name, const glm::vec2& value) const {
    bind();
    const GLint location = get_uniform_location(name);
    if (location >= 0) {
        glUniform2f(location, value.x, value.y);
    }
}

void Shader::set_uniform_vec3(std::string_view name, const glm::vec3& value) const {
    bind();
    const GLint location = get_uniform_location(name);
    if (location >= 0) {
        glUniform3f(location, value.x, value.y, value.z);
    }
}

void Shader::set_uniform_vec4(std::string_view name, const glm::vec4& value) const {
    bind();
    const GLint location = get_uniform_location(name);
    if (location >= 0) {
        glUniform4f(location, value.x, value.y, value.z, value.w);
    }
}

void Shader::set_uniform_mat4(std::string_view name, const glm::mat4& value) const {
    bind();
    const GLint location = get_uniform_location(name);
    if (location >= 0) {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }
}

void Shader::bind() const {
    glUseProgram(m_renderer_id);
}

void Shader::unbind() const {
    glUseProgram(0);
}

} // namespace ngin

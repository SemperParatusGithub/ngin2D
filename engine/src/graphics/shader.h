#pragma once

#include "core/types.h"

#include <string_view>

namespace ngin {

class Shader {
public:
    Shader(std::string_view vertex_source, std::string_view fragment_source);
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    void bind() const;
    void unbind() const;

    RendererID id() const { return m_renderer_id; }

private:
    RendererID m_renderer_id;
};

} // namespace ngin

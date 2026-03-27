#pragma once

#include "core/types.h"

namespace ngin {

class VertexBuffer {
public:
    VertexBuffer(const void* data, u32 size);
    ~VertexBuffer();

    VertexBuffer(const VertexBuffer&) = delete;
    VertexBuffer& operator=(const VertexBuffer&) = delete;

    VertexBuffer(VertexBuffer&& other) noexcept;
    VertexBuffer& operator=(VertexBuffer&& other) noexcept;

    bool is_valid() const { return m_renderer_id != 0; }
    renderer_id get_id() const { return m_renderer_id; }

    void bind() const;
    void unbind() const;

private:
    renderer_id m_renderer_id;
};

} // namespace ngin

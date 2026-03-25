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

    void bind() const;
    void unbind() const;

    RendererID id() const { return m_renderer_id; }

private:
    RendererID m_renderer_id;
};

} // namespace ngin

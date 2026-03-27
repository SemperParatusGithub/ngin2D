#pragma once

#include "core/types.h"

namespace ngin {

class IndexBuffer {
public:
    IndexBuffer(const u32* indices, u32 count);
    ~IndexBuffer();

    IndexBuffer(const IndexBuffer&) = delete;
    IndexBuffer& operator=(const IndexBuffer&) = delete;

    IndexBuffer(IndexBuffer&& other) noexcept;
    IndexBuffer& operator=(IndexBuffer&& other) noexcept;

    bool is_valid() const { return m_renderer_id != 0; }
	renderer_id get_id() const { return m_renderer_id; }

    void bind() const;
    void unbind() const;

    u32 get_count() const { return m_count; }

private:
    renderer_id m_renderer_id;
    u32 m_count;
};

} // namespace ngin

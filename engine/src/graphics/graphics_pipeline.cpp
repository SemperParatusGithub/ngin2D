#include "graphics/graphics_pipeline.h"

#include "core/assert.h"
#include "core/log.h"

#include <glad/glad.h>
#include <utility>

namespace {

constexpr ngin::u32 vertex_format_size(ngin::VertexFormat format) {
    switch (format) {
        case ngin::VertexFormat::float1: return static_cast<ngin::u32>(sizeof(float));
        case ngin::VertexFormat::float2: return static_cast<ngin::u32>(sizeof(float) * 2);
        case ngin::VertexFormat::float3: return static_cast<ngin::u32>(sizeof(float) * 3);
        case ngin::VertexFormat::float4: return static_cast<ngin::u32>(sizeof(float) * 4);
        case ngin::VertexFormat::u8_4: return static_cast<ngin::u32>(sizeof(ngin::u8) * 4);
        default: return 0;
    }
}

constexpr GLint vertex_format_component_count(ngin::VertexFormat format) {
    switch (format) {
        case ngin::VertexFormat::float1: return 1;
        case ngin::VertexFormat::float2: return 2;
        case ngin::VertexFormat::float3: return 3;
        case ngin::VertexFormat::float4: return 4;
        case ngin::VertexFormat::u8_4: return 4;
        default: return 0;
    }
}

constexpr GLenum vertex_format_gl_type(ngin::VertexFormat format) {
    switch (format) {
        case ngin::VertexFormat::float1:
        case ngin::VertexFormat::float2:
        case ngin::VertexFormat::float3:
        case ngin::VertexFormat::float4:
            return GL_FLOAT;
        case ngin::VertexFormat::u8_4:
            return GL_UNSIGNED_BYTE;
        default:
            return 0;
    }
}

} // namespace

namespace ngin {

u32 GraphicsPipeline::Layout::CalculateStride() const {
    u32 stride = 0;
    for (const auto& attribute : attributes) {
        stride += vertex_format_size(attribute.format);
    }
    return stride;
}

u32 GraphicsPipeline::Layout::CalculateOffset(const Attribute& attribute) const {
    u32 offset = 0;
    for (const auto& current_attribute : attributes) {
        if (&current_attribute == &attribute) {
            break;
        }
        offset += vertex_format_size(current_attribute.format);
    }
    return offset;
}

GraphicsPipeline::GraphicsPipeline(
    std::span<const std::byte> vertex_bytes,
    std::span<const u32> indices,
    Layout layout
)
    : GraphicsPipeline(
          std::make_shared<VertexBuffer>(vertex_bytes.data(), static_cast<u32>(vertex_bytes.size_bytes())),
          std::make_shared<IndexBuffer>(indices.data(), static_cast<u32>(indices.size())),
          layout
      ) {
    NGIN_ASSERT_MSG(!vertex_bytes.empty(), "GraphicsPipeline requires vertex byte data.");
    NGIN_ASSERT_MSG(!indices.empty(), "GraphicsPipeline requires index data.");
}

GraphicsPipeline::GraphicsPipeline(const std::shared_ptr<VertexBuffer>& vertex_buffer, Layout layout)
    : GraphicsPipeline(vertex_buffer, std::shared_ptr<IndexBuffer>{}, layout) {}

GraphicsPipeline::GraphicsPipeline(
    const std::shared_ptr<VertexBuffer>& vertex_buffer,
    const std::shared_ptr<IndexBuffer>& index_buffer,
    Layout layout
)
    : vertex_buffer(vertex_buffer),
      index_buffer(index_buffer),
      layout(std::move(layout)),
      m_vertex_array(0) {
    NGIN_ASSERT_MSG(this->vertex_buffer != nullptr, "GraphicsPipeline requires a valid vertex buffer.");

    glGenVertexArrays(1, &m_vertex_array);
    glBindVertexArray(m_vertex_array);

    if (this->vertex_buffer) {
        this->vertex_buffer->bind();

        for (std::size_t i = 0; i < this->layout.attributes.size(); ++i) {
            const auto& attribute = this->layout.attributes[i];
            const GLint component_count = vertex_format_component_count(attribute.format);
            const GLenum gl_type = vertex_format_gl_type(attribute.format);
            NGIN_ASSERT_MSG(component_count > 0, "Invalid vertex format in pipeline layout.");
            NGIN_ASSERT_MSG(gl_type != 0, "Invalid OpenGL vertex attribute type.");

            glEnableVertexAttribArray(static_cast<GLuint>(i));
            glVertexAttribPointer(
                static_cast<GLuint>(i),
                component_count,
                gl_type,
                attribute.normalized ? GL_TRUE : GL_FALSE,
                static_cast<GLsizei>(this->layout.CalculateStride()),
                reinterpret_cast<void*>(
                    static_cast<uintptr_t>(this->layout.CalculateOffset(attribute))
                )
            );

            NGIN_TRACE("---------------------------------------------------------");
            NGIN_TRACE("Index: {}", i);
            NGIN_TRACE("Count: {}", component_count);
            NGIN_TRACE("Type: {}", gl_type == GL_FLOAT ? "float" : "other");
            NGIN_TRACE("Normalized: {}", attribute.normalized ? 1 : 0);
            NGIN_TRACE("Stride: {}", this->layout.CalculateStride());
            NGIN_TRACE("Offset: {}", this->layout.CalculateOffset(attribute));
        }
        NGIN_TRACE("---------------------------------------------------------");
    }

    if (this->index_buffer) {
        this->index_buffer->bind();
    }

    glBindVertexArray(0);
}

GraphicsPipeline::~GraphicsPipeline() {
    if (m_vertex_array != 0) {
        glDeleteVertexArrays(1, &m_vertex_array);
        m_vertex_array = 0;
    }
}

void GraphicsPipeline::bind() const {
    glBindVertexArray(m_vertex_array);
}

void GraphicsPipeline::unbind() const {
    glBindVertexArray(0);
}

} // namespace ngin

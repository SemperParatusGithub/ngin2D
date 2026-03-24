#pragma once

#include "graphics/index_buffer.h"
#include "graphics/vertex_buffer.h"
#include "core/assert.h"

#include <cstddef>
#include <initializer_list>
#include <memory>
#include <span>
#include <string>
#include <utility>
#include <vector>

namespace ngin {

enum class VertexFormat {
    float1,
    float2,
    float3,
    float4,
    u8_4
};

class GraphicsPipeline {
public:
    struct Layout {
        struct Attribute {
            std::string name;
            VertexFormat format;
            bool normalized = false;
        };

        Layout() = default;
        Layout(std::initializer_list<Attribute> attributes_init) : attributes(attributes_init) {}

        std::vector<Attribute> attributes;

        u32 CalculateStride() const;
        u32 CalculateOffset(const Attribute& attribute) const;
    };

    GraphicsPipeline(
        const std::shared_ptr<VertexBuffer>& vertex_buffer,
        const std::shared_ptr<IndexBuffer>& index_buffer,
        Layout layout
    );
    GraphicsPipeline(
        std::span<const std::byte> vertex_bytes,
        std::span<const u32> indices,
        Layout layout = {}
    );
    GraphicsPipeline(const std::shared_ptr<VertexBuffer>& vertex_buffer, Layout layout);
    template <typename VertexT>
    GraphicsPipeline(
        std::span<const VertexT> vertices,
        std::span<const u32> indices,
        Layout layout = {}
    )
        : GraphicsPipeline(
              std::as_bytes(vertices),
              indices,
              ValidateLayoutForVertex(
                  std::move(layout),
                  static_cast<u32>(sizeof(VertexT)),
                  static_cast<u32>(vertices.size_bytes())
              )
          ) {
        NGIN_ASSERT_MSG(!vertices.empty(), "GraphicsPipeline requires at least one vertex.");
        NGIN_ASSERT_MSG(!indices.empty(), "GraphicsPipeline requires at least one index.");
    }
    ~GraphicsPipeline();

    GraphicsPipeline(const GraphicsPipeline&) = delete;
    GraphicsPipeline& operator=(const GraphicsPipeline&) = delete;
    GraphicsPipeline(GraphicsPipeline&& other) = delete;
    GraphicsPipeline& operator=(GraphicsPipeline&& other) = delete;

    void bind() const;
    void unbind() const;

    std::shared_ptr<VertexBuffer> vertex_buffer;
    std::shared_ptr<IndexBuffer> index_buffer;
    Layout layout;

private:
    static Layout ValidateLayoutForVertex(Layout layout, u32 vertex_element_size, u32 vertex_buffer_size) {
        if (!layout.attributes.empty()) {
            const u32 stride = layout.CalculateStride();
			NGIN_ASSERT_MSG(
				stride >= vertex_element_size,
				"Pipeline layout stride is smaller than vertex element size."
			);
			NGIN_ASSERT_MSG(
				stride % vertex_element_size == 0,
				"Pipeline layout stride is not aligned to vertex element size."
			);
			NGIN_ASSERT_MSG(
				vertex_buffer_size % stride == 0,
				"Vertex buffer size is not divisible by pipeline layout stride."
			);
        }
        return layout;
    }

    RendererID m_vertex_array;
};

} // namespace ngin

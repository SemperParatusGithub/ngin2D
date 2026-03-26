#pragma once

#include "graphics/index_buffer.h"
#include "graphics/vertex_buffer.h"
#include "core/assert.h"

#include <cstddef>
#include <initializer_list>
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
		ref<VertexBuffer> vertex_buffer,
		ref<IndexBuffer> index_buffer,
        const Layout& layout
	);
    GraphicsPipeline(
        std::span<const std::byte> vertex_bytes,
        std::span<const u32> indices,
        const Layout& layout
    );

    template <typename VertexT>
    GraphicsPipeline(
        std::span<const VertexT> vertices,
        std::span<const u32> indices,
        Layout layout = {}
    ) {
        NGIN_ASSERT_MSG(!vertices.empty(), "GraphicsPipeline requires at least one vertex.");
        NGIN_ASSERT_MSG(!indices.empty(), "GraphicsPipeline requires at least one index.");

        const Layout validated_layout = ValidateLayoutForVertex(
            std::move(layout),
            static_cast<u32>(sizeof(VertexT)),
            static_cast<u32>(vertices.size_bytes())
        );

        m_vertex_array = 0;
        m_vertex_buffer = create_ref<VertexBuffer>(
            std::as_bytes(vertices).data(),
            static_cast<u32>(vertices.size_bytes())
        );
        m_index_buffer =
            create_ref<IndexBuffer>(indices.data(), static_cast<u32>(indices.size()));
        m_layout = validated_layout;

        setup_pipeline();
    }

    ~GraphicsPipeline();

    GraphicsPipeline(const GraphicsPipeline&) = delete;
    GraphicsPipeline& operator=(const GraphicsPipeline&) = delete;
    GraphicsPipeline(GraphicsPipeline&& other) = delete;
    GraphicsPipeline& operator=(GraphicsPipeline&& other) = delete;

    void setup_pipeline();

    void bind() const;
    void unbind() const;

    u32 get_index_count() const { return m_index_buffer->count(); }

private:
    ref<VertexBuffer> m_vertex_buffer;
    ref<IndexBuffer> m_index_buffer;
    Layout m_layout;

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

    renderer_id m_vertex_array;
};

} // namespace ngin

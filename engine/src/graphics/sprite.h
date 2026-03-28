#pragma once 

#include "core/types.h"
#include "graphics/texture.h"
#include "transform.h"

#include <glm/glm.hpp>
#include <array>

namespace ngin {

class Sprite {
public:
	Sprite();
	Sprite(ref<Texture> texture);
	~Sprite();

	Transform& get_transform() { return m_transform; }
	const Transform& get_transform() const { return m_transform; }

	void set_texture(ref<Texture> texture);
	ref<Texture> get_texture() const { return m_texture; }

	const std::array<glm::vec2, 4>& get_texture_coords() const { return m_texture_coords; }


private:
	Transform m_transform;
	ref<Texture> m_texture;
	std::array<glm::vec2, 4> m_texture_coords;
	// TODO: tint_color
	// TODO: sprite sheets

private:
	void recalculate_texture_coords();
};

} // namespace ngin
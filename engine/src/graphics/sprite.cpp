#include "sprite.h"


namespace ngin {

Sprite::Sprite() {
	recalculate_texture_coords();
}

Sprite::Sprite(ref<Texture> texture) : 
	m_texture(texture) {
	recalculate_texture_coords();
}

Sprite::~Sprite() {
}

void Sprite::set_texture(ref<Texture> texture) {
	m_texture = texture;

	recalculate_texture_coords();
}

void Sprite::recalculate_texture_coords() {
	m_texture_coords[0] = { 0.0f, 0.0f };
	m_texture_coords[1] = { 1.0f, 0.0f };
	m_texture_coords[2] = { 1.0f, 1.0f };
	m_texture_coords[3] = { 0.0f, 1.0f };
}

} // namespace ngin
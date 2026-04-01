#pragma once

#include "core/types.h"
#include "graphics/sprite.h"

#include <glm/glm.hpp>

namespace ngin {

struct SpriteComponent {
	SpriteComponent() = default;
	SpriteComponent(const glm::vec4& color);

	ref<Texture> texture;
	glm::vec4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
};

}
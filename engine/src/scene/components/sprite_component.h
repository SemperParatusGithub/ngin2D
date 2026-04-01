#pragma once

#include "core/types.h"
#include "graphics/sprite.h"

#include <glm/glm.hpp>

namespace ngin {

// NOTE: This is intentionally minimal for now and only stores per-entity sprite data
// that composes with TransformComponent: a texture reference plus tint color.
//
// Keep this as ECS-facing render data rather than growing it around the older Sprite
// class, which also owns transform state and texture coordinates. As sprite sheets /
// atlases are added, this component and the rendering API should move toward explicit
// sub-texture or frame data instead of assuming a single whole-texture sprite.
struct SpriteComponent {
	SpriteComponent() = default;
	explicit SpriteComponent(const glm::vec4& color) : color(color) {}

	ref<Texture> texture;
	glm::vec4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
};

}
#pragma once
#include "transform.h"


namespace ngin {

struct TransformComponent {
	TransformComponent() = default;
	TransformComponent(const Transform& transform) : transform(transform) {}

	Transform transform;
};

} // namespace ngin
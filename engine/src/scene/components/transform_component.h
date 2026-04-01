#pragma once
#include "transform.h"


namespace ngin {

struct TransformComponent {
	TransformComponent() = default;

	Transform transform;
};

} // namespace ngin
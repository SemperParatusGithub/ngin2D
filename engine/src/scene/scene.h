#pragma once

#include "core/types.h"

#include <string>
#include <string_view>

#include <entt/entt.hpp>

namespace ngin {

class Entity;

class Scene {
public:
	Scene() = default;
	~Scene() = default;

	const entt::registry& get_registry() const { return m_registry; }
	entt::registry& get_registry() { return m_registry; }

	Entity create_entity();
	Entity create_entity(std::string_view tag);
	void destroy_entity(Entity entity);

private:
	entt::registry m_registry;
	internal_entity_id m_next_id = 0;
};

} // namespace ngin

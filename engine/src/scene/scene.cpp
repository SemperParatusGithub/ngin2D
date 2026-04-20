#include "scene.h"

#include "entity.h"

#include "core/assert.h"
#include "components/components.h"

namespace ngin {
Entity Scene::create_entity()
{
	std::string tag = "Entity " + std::to_string(m_next_id);
	return create_entity(tag);
}
Entity Scene::create_entity(std::string_view tag) {
	auto handle = m_registry.create();
	m_registry.emplace<IDComponent>(handle, m_next_id++);
	m_registry.emplace<TagComponent>(handle, tag);

	return Entity{ handle, this };
}

Entity Scene::create_entity_with_id(internal_entity_id id, std::string_view tag) {
	auto handle = m_registry.create();
	m_registry.emplace<IDComponent>(handle, id);
	m_registry.emplace<TagComponent>(handle, tag);

	if (id >= m_next_id) {
		m_next_id = id + 1;
	}

	return Entity{ handle, this };
}

void Scene::destroy_entity(Entity entity) {
	NGIN_ASSERT_MSG(entity.get_scene() == this, "Attempted to destroy entity from a different scene");
	NGIN_ASSERT_MSG(entity.is_valid(), "Attempted to destroy an invalid entity");
	m_registry.destroy(entity.get_handle());
}

void Scene::clear() {
	m_registry.clear();
	m_next_id = 0;
}

} // namespace ngin

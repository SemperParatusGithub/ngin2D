#include "scene.h"

#include "entity.h"

#include "components/components.h"

namespace ngin {

Entity Scene::create_entity(std::string_view tag) {
	auto handle = m_registry.create();
	m_registry.emplace<IDComponent>(handle, m_next_id++);
	m_registry.emplace<TagComponent>(handle, tag);

	return Entity{ handle, this };
}

void Scene::destroy_entity(Entity entity) {
	if (entity.is_valid() && entity.get_scene() == this)
		m_registry.destroy(entity.get_handle());
}

} // namespace ngin

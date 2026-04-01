#include "entity.h"

#include "scene.h"

#include <entt/entity/entity.hpp>

namespace ngin {

Entity::Entity() : 
	m_handle(entt::null),
	m_scene(nullptr) {

}
Entity::Entity(entt::entity entity, Scene* scene) : 
	m_handle(entity),
	m_scene(scene) {

}
Entity::~Entity() {
	m_handle = entt::null;
	m_scene = nullptr;
}

bool Entity::is_valid() const {
	if (!m_scene || m_handle == entt::null)
		return false;
	return m_scene->get_registry().valid(m_handle);
}
Entity::operator bool() const {
	return is_valid();
}

entt::entity Entity::get_handle() const {
	return m_handle;
}
Scene* Entity::get_scene() const {
	return m_scene;
}

} // namespace ngin
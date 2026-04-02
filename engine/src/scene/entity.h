#pragma once 

#include "core/assert.h"
#include "scene.h"

#include <entt/entt.hpp>
#include <utility>

namespace ngin {

class Entity {
public:
	Entity();
	Entity(entt::entity entity, Scene* scene);
	~Entity();

	bool is_valid() const;
	explicit operator bool() const;

	bool operator==(const Entity& other) const {
		return m_scene == other.m_scene && m_handle == other.m_handle;
	}
	bool operator!=(const Entity& other) const { return !(*this == other); }

	entt::entity get_handle() const;
	Scene* get_scene() const;

	template<typename Component, typename... Args>
	decltype(auto) emplace(Args&&... args) {
		assert_valid_entity();
		NGIN_ASSERT_MSG(!has<Component>(), "Entity already has component");
		return m_scene->get_registry().emplace<Component>(m_handle, std::forward<Args>(args)...);
	}

	template<typename Component, typename... Args>
	decltype(auto) emplace_or_replace(Args&&... args) {
		assert_valid_entity();
		return m_scene->get_registry().emplace_or_replace<Component>(m_handle, std::forward<Args>(args)...);
	}

	template<typename... Components>
	std::size_t remove() {
		assert_valid_entity();
		return m_scene->get_registry().remove<Components...>(m_handle);
	}

	template<typename ... Components>
	bool has() const {
		assert_valid_entity();
		return m_scene->get_registry().all_of<Components...>(m_handle);
	}

	template<typename... Components>
	decltype(auto) get() {
		assert_valid_entity();
		NGIN_ASSERT_MSG(has<Components...>(), "Entity missing requested component");
		return m_scene->get_registry().get<Components ...>(m_handle);
	}

	template<typename... Components>
	decltype(auto) get() const {
		assert_valid_entity();
		NGIN_ASSERT_MSG(has<Components...>(), "Entity missing requested component");
		return m_scene->get_registry().get<Components ...>(m_handle);
	}

	template<typename... Components>
	decltype(auto) try_get() {
		assert_valid_entity();
		return m_scene->get_registry().try_get<Components...>(m_handle);
	}

	template<typename... Components>
	decltype(auto) try_get() const {
		assert_valid_entity();
		return m_scene->get_registry().try_get<Components...>(m_handle);
	}

private:
	void assert_valid_entity() const {
		NGIN_ASSERT_MSG(m_scene != nullptr, "Entity has no owning scene");
		NGIN_ASSERT_MSG(m_handle != entt::null, "Entity handle is null");
		NGIN_ASSERT_MSG(m_scene->get_registry().valid(m_handle), "Entity handle is not valid in scene registry");
	}

private:
	entt::entity m_handle{ entt::null };
	Scene* m_scene{ nullptr };
};

} // namespace ngin

#pragma once 

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

	entt::entity get_handle() const;
	Scene* get_scene() const;

	template<typename Component, typename...Args>
	decltype(auto) emplace(Args&&...args) {
		return m_scene->get_registry().emplace<Component>(m_handle, std::forward<Args>(args)...);
	}
	template<typename...Components>
	std::size_t remove() {
		return m_scene->get_registry().remove<Components...>(m_handle);
	}

	template<typename ... Components>
	bool has() const {
		return m_scene->get_registry().all_of<Components...>(m_handle);
	}

	template<typename...Components>
	decltype(auto) get() {
		return m_scene->get_registry().get<Components ...>(m_handle);
	}
	template<typename...Components>
	decltype(auto) get() const {
		return m_scene->get_registry().get<Components ...>(m_handle);
	}

private:
	entt::entity m_handle{ entt::null };
	Scene* m_scene{ nullptr };
};

} // namespace ngin

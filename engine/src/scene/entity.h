#pragma once 
#include <entt/entt.hpp>

namespace ngin {

struct Scene;

class Entity {
public:
	Entity();
	Entity(entt::entity entity, Scene* scene);
	~Entity();

	bool is_valid() const;
	explicit operator bool() const;

	entt::entity get_handle() const;
	Scene* get_scene() const;

private:
	entt::entity m_handle{ entt::null };
	Scene* m_scene{ nullptr };
};

} // namespace ngin

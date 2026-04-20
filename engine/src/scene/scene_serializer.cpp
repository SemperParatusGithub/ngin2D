#include "scene_serializer.h"

#include "core/log.h"
#include "scene/entity.h"
#include "scene/scene.h"
#include "scene/components/components.h"
#include "transform.h"

#include <algorithm>
#include <fstream>
#include <utility>
#include <vector>

#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

namespace ngin {

namespace {

nlohmann::json to_json(const glm::vec3& v) {
	return nlohmann::json::array({ v.x, v.y, v.z });
}

nlohmann::json to_json(const glm::vec4& v) {
	return nlohmann::json::array({ v.x, v.y, v.z, v.w });
}

bool try_read_vec3(const nlohmann::json& j, glm::vec3& out) {
	if (!j.is_array() || j.size() != 3) {
		return false;
	}
	out = glm::vec3{
		j[0].get<float>(),
		j[1].get<float>(),
		j[2].get<float>()
	};
	return true;
}

bool try_read_vec4(const nlohmann::json& j, glm::vec4& out) {
	if (!j.is_array() || j.size() != 4) {
		return false;
	}
	out = glm::vec4{
		j[0].get<float>(),
		j[1].get<float>(),
		j[2].get<float>(),
		j[3].get<float>()
	};
	return true;
}

nlohmann::json serialize_transform(const Transform& transform) {
	nlohmann::json j;
	j[SceneSerializer::Json::transform_position] = to_json(transform.get_position());
	j[SceneSerializer::Json::transform_rotation] = to_json(transform.get_rotation());
	j[SceneSerializer::Json::transform_scale] = to_json(transform.get_scale());
	return j;
}

void deserialize_transform(const nlohmann::json& j, Transform& out) {
	glm::vec3 position{ 0.0f };
	glm::vec3 rotation{ 0.0f };
	glm::vec3 scale{ 1.0f };

	if (const auto it = j.find(SceneSerializer::Json::transform_position); it != j.end()) {
		try_read_vec3(*it, position);
	}
	if (const auto it = j.find(SceneSerializer::Json::transform_rotation); it != j.end()) {
		try_read_vec3(*it, rotation);
	}
	if (const auto it = j.find(SceneSerializer::Json::transform_scale); it != j.end()) {
		try_read_vec3(*it, scale);
	}

	out.set_position(position);
	out.set_rotation(rotation);
	out.set_scale(scale);
}

nlohmann::json serialize_sprite(const SpriteComponent& sprite) {
	// NOTE: texture is intentionally not serialized yet. `Texture` does not track
	// the path it was loaded from, and sprites are not yet driven through an
	// asset-reference system. Once textures are asset-managed, add the texture
	// reference (by GUID or relative path) here.
	nlohmann::json j;
	j[SceneSerializer::Json::sprite_color] = to_json(sprite.color);
	return j;
}

void deserialize_sprite(const nlohmann::json& j, SpriteComponent& out) {
	if (const auto it = j.find(SceneSerializer::Json::sprite_color); it != j.end()) {
		try_read_vec4(*it, out.color);
	}
}

} // namespace

bool SceneSerializer::serialize(const Scene& scene, const std::filesystem::path& path) {
	nlohmann::json j;
	j[Json::version] = current_version;

	auto entities = nlohmann::json::array();

	const entt::registry& registry = scene.get_registry();
	// Iterate entities through the IDComponent storage; every entity has one.
	// Use a reverse traversal to preserve stable creation order on disk, since
	// EnTT's view iterates in reverse of the underlying sparse set order.
	auto id_view = registry.view<const IDComponent>();
	std::vector<entt::entity> ordered(id_view.begin(), id_view.end());
	std::sort(ordered.begin(), ordered.end(), [&](entt::entity a, entt::entity b) {
		return registry.get<IDComponent>(a).id < registry.get<IDComponent>(b).id;
	});

	for (entt::entity handle : ordered) {
		nlohmann::json entity_json;
		entity_json[Json::entity_id] = registry.get<IDComponent>(handle).id;

		if (const auto* tag = registry.try_get<TagComponent>(handle)) {
			entity_json[Json::entity_tag] = tag->tag;
		} else {
			entity_json[Json::entity_tag] = std::string{};
		}

		nlohmann::json components = nlohmann::json::object();
		if (const auto* tc = registry.try_get<TransformComponent>(handle)) {
			components[Json::component_transform] = serialize_transform(tc->transform);
		}
		if (const auto* sc = registry.try_get<SpriteComponent>(handle)) {
			components[Json::component_sprite] = serialize_sprite(*sc);
		}
		entity_json[Json::entity_components] = std::move(components);

		entities.push_back(std::move(entity_json));
	}

	j[Json::entities] = std::move(entities);

	std::ofstream out(path);
	if (!out) {
		NGIN_ERROR("SceneSerializer: failed to open '{}' for writing", path.generic_string());
		return false;
	}
	out << j.dump(4);
	return static_cast<bool>(out);
}

bool SceneSerializer::deserialize(Scene& scene, const std::filesystem::path& path) {
	std::ifstream in(path);
	if (!in) {
		NGIN_ERROR("SceneSerializer: failed to open '{}' for reading", path.generic_string());
		return false;
	}

	nlohmann::json j;
	try {
		in >> j;
	} catch (const nlohmann::json::parse_error& e) {
		NGIN_ERROR("SceneSerializer: JSON parse error in '{}': {}", path.generic_string(), e.what());
		return false;
	}

	const int version = j.value(Json::version, 0);
	if (version != current_version) {
		NGIN_ERROR(
			"SceneSerializer: unsupported scene version {} in '{}' (expected {})",
			version, path.generic_string(), current_version);
		return false;
	}

	const auto entities_it = j.find(Json::entities);
	if (entities_it == j.end() || !entities_it->is_array()) {
		NGIN_ERROR("SceneSerializer: missing or invalid 'entities' array in '{}'", path.generic_string());
		return false;
	}

	// Clear the existing scene before loading so deserialization always produces
	// a scene that matches the file exactly, rather than merging on top.
	scene.clear();

	for (const auto& entity_json : *entities_it) {
		if (!entity_json.is_object()) {
			continue;
		}

		const auto id = entity_json.value(Json::entity_id, internal_entity_id{ 0 });
		const auto tag = entity_json.value(Json::entity_tag, std::string{});

		Entity entity = scene.create_entity_with_id(id, tag);

		const auto components_it = entity_json.find(Json::entity_components);
		if (components_it == entity_json.end() || !components_it->is_object()) {
			continue;
		}

		if (const auto it = components_it->find(Json::component_transform); it != components_it->end()) {
			Transform transform;
			deserialize_transform(*it, transform);
			entity.emplace_or_replace<TransformComponent>(transform);
		}
		if (const auto it = components_it->find(Json::component_sprite); it != components_it->end()) {
			SpriteComponent sprite;
			deserialize_sprite(*it, sprite);
			entity.emplace_or_replace<SpriteComponent>(sprite);
		}
	}

	return true;
}

} // namespace ngin

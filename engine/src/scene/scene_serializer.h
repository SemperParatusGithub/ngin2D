#pragma once

#include <filesystem>

namespace ngin {

class Scene;

class SceneSerializer {
public:
	struct Json {
		static constexpr const char* version = "version";
		static constexpr const char* entities = "entities";

		static constexpr const char* entity_id = "id";
		static constexpr const char* entity_tag = "tag";
		static constexpr const char* entity_components = "components";

		static constexpr const char* component_transform = "transform";
		static constexpr const char* component_sprite = "sprite";

		static constexpr const char* transform_position = "position";
		static constexpr const char* transform_rotation = "rotation";
		static constexpr const char* transform_scale = "scale";

		static constexpr const char* sprite_color = "color";
	};

	// Bump when an incompatible on-disk change is made. Deserialize rejects
	// unknown versions rather than silently loading mismatched data.
	static constexpr int current_version = 1;

	static bool serialize(const Scene& scene, const std::filesystem::path& path);
	static bool deserialize(Scene& scene, const std::filesystem::path& path);
};

} // namespace ngin

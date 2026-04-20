#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace ngin {

class Project {
public:
	struct Json {
		static constexpr const char* project_name = "project_name";
		static constexpr const char* root_directory = "root_directory";
		static constexpr const char* relative_asset_directory = "relative_asset_directory";
		static constexpr const char* relative_scenes_directory = "relative_scenes_directory";
		static constexpr const char* relative_default_scene = "relative_default_scene";
	};

	Project() = default;
	Project(std::string_view project_name);
	~Project() = default;

	void set_project_name(std::string_view name);
	const std::string& get_project_name() const;

	void set_root_directory(std::filesystem::path root_dir);
	const std::filesystem::path& get_root_directory() const;

	void set_relative_asset_directory(std::filesystem::path relative_assert_dir);
	const std::filesystem::path& get_relative_asset_directory() const;
	std::filesystem::path get_asset_directory() const;

	void set_relative_scenes_directory(std::filesystem::path relative_scenes_dir);
	const std::filesystem::path& get_relative_scenes_directory() const;
	std::filesystem::path get_scenes_directory() const;

	void set_relative_default_scene(std::filesystem::path relative_default_scene);
	const std::filesystem::path& get_realitve_default_scene() const;
	std::filesystem::path get_default_scene() const;

	// Recursively scan the configured scenes folder for `.scene` files. Returned
	// paths are relative to `get_scenes_directory()` (so they can serve as stable
	// scene identifiers within the project) and sorted lexicographically for
	// deterministic ordering. Returns an empty vector if the root or scenes
	// directory is not configured, the folder does not exist on disk, or it
	// cannot be read. Does not throw.
	std::vector<std::filesystem::path> enumerate_scenes() const;

private:
	std::string m_project_name = "Untitled Project";
	std::filesystem::path m_root_directory;
	std::filesystem::path m_asset_directory;	// asset directory relative to root directory
	std::filesystem::path m_scenes_directory;	// scenes directory relative to root directory
	std::filesystem::path m_default_scene;		// default scene path relative to root directory
};

} // namespace ngin
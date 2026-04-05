#pragma once

#include <string>
#include <filesystem>

namespace ngin {

class Project {
public:
	struct Json {
		static constexpr const char* project_name = "project_name";
		static constexpr const char* root_directory = "root_directory";
		static constexpr const char* relative_asset_directory = "relative_asset_directory";
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
	const std::filesystem::path& get_asset_directory() const;

	void set_relative_default_scene(std::filesystem::path relative_default_scene);
	const std::filesystem::path& get_realitve_default_scene() const;
	const std::filesystem::path& get_default_scene() const;

private:
	std::string m_project_name = "Untitled Project";
	std::filesystem::path m_root_directory;
	std::filesystem::path m_asset_directory;	// asset directory relative to root directory
	std::filesystem::path m_default_scene;		// default scene directory relative to root directory
};

} // namespace ngin
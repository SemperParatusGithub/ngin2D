#include "project.h"


namespace ngin {

Project::Project(std::string_view project_name) : 
	m_project_name(project_name) {

}

void Project::set_project_name(std::string_view name) {
	m_project_name = name;
}
const std::string& Project::get_project_name() const {
	return m_project_name;
}

void Project::set_root_directory(std::filesystem::path root_dir) {
	m_root_directory = root_dir;
}
const std::filesystem::path& Project::get_root_directory() const {
	return m_root_directory;
}

void Project::set_relative_asset_directory(std::filesystem::path relative_assert_dir) {
	m_asset_directory = relative_assert_dir;
}
const std::filesystem::path& Project::get_relative_asset_directory() const {
	return m_asset_directory;
}
std::filesystem::path Project::get_asset_directory() const {
	return m_root_directory / m_asset_directory;
}

void Project::set_relative_default_scene(std::filesystem::path relative_default_scene) {
	m_default_scene = relative_default_scene;
}
const std::filesystem::path& Project::get_realitve_default_scene() const {
	return m_default_scene;
}
std::filesystem::path Project::get_default_scene() const {
	return m_root_directory / m_default_scene;
}

} // namespace ngin
#include "project.h"

#include <algorithm>
#include <cctype>
#include <system_error>


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

void Project::set_relative_scenes_directory(std::filesystem::path relative_scenes_dir) {
	m_scenes_directory = relative_scenes_dir;
}
const std::filesystem::path& Project::get_relative_scenes_directory() const {
	return m_scenes_directory;
}
std::filesystem::path Project::get_scenes_directory() const {
	return m_root_directory / m_scenes_directory;
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

std::vector<std::filesystem::path> Project::enumerate_scenes() const {
	std::vector<std::filesystem::path> result;

	// Refuse to scan when either path is unset. Without this guard,
	// `get_scenes_directory()` would resolve to `m_root_directory` itself (or
	// worse, a relative "scenes" interpreted against the process cwd) and
	// silently pull in unrelated files.
	if (m_root_directory.empty() || m_scenes_directory.empty()) {
		return result;
	}

	const std::filesystem::path scenes_root = get_scenes_directory();

	std::error_code ec;
	if (!std::filesystem::exists(scenes_root, ec) || ec) {
		return result;
	}

	std::filesystem::recursive_directory_iterator it(
		scenes_root,
		std::filesystem::directory_options::skip_permission_denied,
		ec);
	if (ec) {
		return result;
	}

	const std::filesystem::recursive_directory_iterator end;
	for (; it != end; it.increment(ec)) {
		if (ec) {
			break;
		}

		std::error_code file_ec;
		if (!it->is_regular_file(file_ec) || file_ec) {
			continue;
		}

		const std::filesystem::path& abs = it->path();
		std::string ext = abs.extension().string();
		std::transform(ext.begin(), ext.end(), ext.begin(),
			[](unsigned char c) { return static_cast<char>(std::tolower(c)); });
		if (ext != ".scene") {
			continue;
		}

		std::error_code rel_ec;
		std::filesystem::path rel = std::filesystem::relative(abs, scenes_root, rel_ec);
		if (rel_ec || rel.empty()) {
			continue;
		}
		result.push_back(std::move(rel));
	}

	std::sort(result.begin(), result.end());
	return result;
}

} // namespace ngin
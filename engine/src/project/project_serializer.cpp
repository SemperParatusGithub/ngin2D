#include "project_serializer.h"
#include "project.h"

#include <fstream>
#include <nlohmann/json.hpp>

namespace ngin {

bool ProjectSerializer::serialize(const Project& project, const std::filesystem::path& path) {
	nlohmann::json j;
	j[Project::Json::project_name] = project.get_project_name();
	j[Project::Json::root_directory] = project.get_root_directory().generic_string();
	j[Project::Json::relative_asset_directory] = project.get_relative_asset_directory().generic_string();
	j[Project::Json::relative_scenes_directory] = project.get_relative_scenes_directory().generic_string();
	j[Project::Json::relative_default_scene] = project.get_realitve_default_scene().generic_string();

	std::ofstream out(path);
	if (!out) {
		return false;
	}
	out << j.dump(4);
	return static_cast<bool>(out);
}

bool ProjectSerializer::deserialize(Project& project, const std::filesystem::path& path) {
	std::ifstream in(path);
	if (!in) {
		return false;
	}

	nlohmann::json j;
	try {
		in >> j;
	} catch (const nlohmann::json::parse_error&) {
		return false;
	}

	project.set_project_name(j.value(Project::Json::project_name, project.get_project_name()));
	project.set_root_directory(j.value(Project::Json::root_directory, std::string{}));
	project.set_relative_asset_directory(j.value(Project::Json::relative_asset_directory, std::string{}));
	project.set_relative_scenes_directory(j.value(Project::Json::relative_scenes_directory, std::string{}));
	project.set_relative_default_scene(j.value(Project::Json::relative_default_scene, std::string{}));

	return true;
}

} // namespace ngin

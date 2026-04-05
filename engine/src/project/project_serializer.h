#pragma once

#include <filesystem>

namespace ngin {

class Project;

class ProjectSerializer {
public:
	static bool serialize(const Project& project, const std::filesystem::path& path);
	static bool deserialize(Project& project, const std::filesystem::path& path);
};

} // namespace ngin

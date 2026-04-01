#pragma once 

#include <string_view>
#include <string>


namespace ngin {

struct TagComponent {
	TagComponent(std::string_view tag) : tag(tag) {}

	std::string tag;
};

} // namespace ngin

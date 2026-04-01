#pragma once 

#include "core/types.h"

namespace ngin {

struct IDComponent {
	IDComponent(internal_entity_id id) : id(id) {}
	
	internal_entity_id id;
};

} // namespace ngin
#pragma once

#include "core/types.h"
#include "key_codes.h"

namespace ngin {

class Input {
public:
    static bool is_key_pressed(KeyCode key);
    static bool is_key_pressed(i32 key);
};

} // namespace ngin

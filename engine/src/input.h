#pragma once

#include "core/types.h"
#include "key_codes.h"

#include <utility>

namespace ngin {

class Input {
public:
    static bool is_key_pressed(KeyCode key);
    static bool is_key_pressed(i32 key);

    static std::pair<f32, f32> mouse_position();
    static f32 mouse_x();
    static f32 mouse_y();
};

} // namespace ngin

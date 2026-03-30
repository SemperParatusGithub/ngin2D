#pragma once

#include "event.h"

#include <cstddef>
#include <optional>
#include <span>
#include <vector>

namespace ngin {

/// Host-agnostic FIFO buffer for `Event` values. GLFW callbacks (or later Qt) push;
/// the application drains via `pop_next()` or views `events()` after a poll boundary.
class EventQueue {
public:
    void clear();
    void push(Event&& event);

    std::span<const Event> events() const;

    /// Returns the next unconsumed event and advances the read cursor, or `nullopt` if none remain.
    std::optional<Event> pop_next();

    bool exhausted() const { return m_read_index >= m_events.size(); }

private:
    std::vector<Event> m_events;
    std::size_t m_read_index = 0;
};

} // namespace ngin

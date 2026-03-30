#include "event_queue.h"

namespace ngin {

void EventQueue::clear() {
    m_events.clear();
    m_read_index = 0;
}

void EventQueue::push(Event&& event) {
    m_events.push_back(std::move(event));
}

std::span<const Event> EventQueue::events() const {
    return std::span<const Event>(m_events.data(), m_events.size());
}

std::optional<Event> EventQueue::pop_next() {
    if (m_read_index >= m_events.size()) {
        return std::nullopt;
    }
    return m_events[m_read_index++];
}

} // namespace ngin

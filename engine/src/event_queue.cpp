#include "event_queue.h"

namespace ngin {

void EventQueue::clear() {
    m_events.clear();
    m_read_index = 0;
}

void EventQueue::push(Event&& event) {
    m_events.push_back(std::move(event));
}

std::optional<Event> EventQueue::pop_next() {
    if (m_read_index >= m_events.size()) {
        if (!m_events.empty()) {
            m_events.clear();
            m_read_index = 0;
        }
        return std::nullopt;
    }
    return std::move(m_events[m_read_index++]);
}

} // namespace ngin

#pragma once

#include "EventDispatcher.hpp"
#include "KeyCode.hpp"

namespace Villain {

    enum class KeyEventType {
        PRESS,
        RELEASE
    };

    /// Dispatched upon pressed or released key
    class KeyboardEvent : public Event {
        public:
            KeyboardEvent(KeyEventType type, KeyCode code) : eventType(type), keyCode(code) {}

            KeyCode getKey() const { return keyCode; }
            bool isPressed() { return eventType == KeyEventType::PRESS; }

            VILLAIN_EVENT_TYPE(KeyboardEvent);

        private:
            KeyEventType eventType;
            KeyCode keyCode;
    };

};

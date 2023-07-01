#ifndef __MOUSE_EVENT__
#define __MOUSE_EVENT__

#include "EventDispatcher.hpp"
#include "KeyCode.hpp"

namespace Villain {

    enum class KeyEventType {
        PRESS,
        RELEASE
    };

    class KeyboardEvent : public Event {
        public:
            KeyboardEvent(KeyEventType type, KeyCode code) : eventType(type), keyCode(code) {}

            KeyCode getKey() const { return keyCode; }
            bool isPressed() { return eventType == KeyEventType::PRESS; }

        private:
            KeyEventType eventType;
            KeyCode keyCode;
    };

};

#endif // __MOUSE_EVENT__

#ifndef __MOUSE_EVENT__
#define __MOUSE_EVENT__

#include "EventDispatcher.hpp"
#include "glm/glm.hpp"

namespace Villain {

    enum class MouseButton {
        LEFT,
        MIDDLE,
        RIGHT
    };

    enum class MouseEventType {
        MOVE,
        CLICK,
        RELEASE,
        SCROLL
    };

    class MouseEvent : public Event {
        public:
            // Mouse move event constructor
            MouseEvent(MouseEventType type, glm::vec2 pos, glm::vec2 off)
                : eventType(type), position(pos), offset(off) {}

            // Mouse click/release event constructor
            MouseEvent(MouseEventType type, MouseButton btn)
                : eventType(type), button(btn) {}

            // Mouse wheel event
            MouseEvent(MouseEventType type, int scrollDelta)
                : eventType(type), wheelDelta(scrollDelta) {}

            MouseEventType getEventType() const { return eventType; }

            const glm::vec2& getMousePos() const { return position; }
            const glm::vec2& getMouseOffset() const { return offset; }

            MouseButton getButton() const { return button; }
            bool isLeftMouseButton() const { return button == MouseButton::LEFT && eventType == MouseEventType::CLICK; }
            bool isMiddleMouseButton() const { return button == MouseButton::MIDDLE && eventType == MouseEventType::CLICK; }
            bool isRightMouseButton() const { return button == MouseButton::RIGHT && eventType == MouseEventType::CLICK; }
            int getMouseWheelDelta() const { return wheelDelta; }

        private:
            MouseEventType eventType;
            MouseButton button;
            glm::vec2 position;
            glm::vec2 offset;
            int wheelDelta;
    };

};

#endif // __MOUSE_EVENT__

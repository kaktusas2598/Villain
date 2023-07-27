#pragma once

#include "EventDispatcher.hpp"

namespace Villain {

    /// Dispatched every time engine window is resized
    class WindowResizeEvent : public Event {
        public:
            WindowResizeEvent(int w, int h) : width(w), height(h) {}

            int getWidth() const { return width; }
            int getHeight() const { return height; }

            VILLAIN_EVENT_TYPE(WindowResizeEvent);
        private:
            int width, height;
    };

};



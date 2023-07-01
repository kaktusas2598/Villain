

#include "EventDispatcher.hpp"

namespace Villain {

    class WindowResizeEvent : public Event {
        public:
            WindowResizeEvent(int w, int h) : width(w), height(h) {}

            int getWidth() const { return width; }
            int getHeight() const { return height; }

        private:
            int width, height;
    };

};



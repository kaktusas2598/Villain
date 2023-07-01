#pragma once

#include <algorithm>
#include <vector>

namespace Villain {

    // Event interface
    class Event {
        public:
            virtual ~Event() {}
    };

    // Event listener interface
    class EventListener {
        public:
            virtual ~EventListener() {}
            virtual void handleEvent(Event& event) = 0;
    };

    // Responsible for keeping track of different event listeners and dispatches events to those listeners
    class EventDispatcher {
        public:
            void registerListener(EventListener* listener) {
                listeners.push_back(listener);
            }
            void unregisterListener(EventListener* listener) {
                listeners.erase(std::remove(listeners.begin(), listeners.end(), listener), listeners.end());
            }

            void dispatchEvent(Event& event) {
                for (auto listener : listeners) {
                    listener->handleEvent(event);
                }
            }

        private:
            std::vector<EventListener*> listeners;
    };
}

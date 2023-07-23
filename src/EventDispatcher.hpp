#pragma once

#include <algorithm>
#include <vector>

namespace Villain {

    /// Base Event interface
    class Event {
        public:
            virtual ~Event() {}
    };

    /// Event listener interface. Implement custom listeners and register them using dispatcher
    class EventListener {
        public:
            virtual ~EventListener() {}
            /// All types of events handled here as long as listener is registered by dispatcher
            virtual void handleEvent(Event& event) = 0;
    };

    /// Responsible for keeping track of different event listeners and dispatches events to those listeners
    class EventDispatcher {
        public:
            /// Call once for every new listener we want to subscribe
            void registerListener(EventListener* listener) {
                listeners.push_back(listener);
            }
            /// Stop listening for events
            void unregisterListener(EventListener* listener) {
                listeners.erase(std::remove(listeners.begin(), listeners.end(), listener), listeners.end());
            }

            /// Must be called every time we want to dispatch an event
            void dispatchEvent(Event& event) {
                for (auto listener : listeners) {
                    listener->handleEvent(event);
                }
            }

            /// Get all registered listeners
            std::vector<EventListener*>& getListeners() { return listeners; }

        private:
            std::vector<EventListener*> listeners;
    };
}

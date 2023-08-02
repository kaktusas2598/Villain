#pragma once

#include <algorithm>
#include <functional>
#include <vector>

/// Macro to make binding event callback function to dispatcher easier
#define BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

/// Macro to generate getType() method for each event class
#define VILLAIN_EVENT_TYPE(Type) \
    virtual EventType getType() const override { \
        return EventType::Type; \
    }

namespace Villain {

    /// Used in every Event class so that dynamic_cast could be avoided
    enum class EventType {
        KeyboardEvent,
        MouseEvent,
        WindowResizeEvent,
        FileSelectedEvent
    };

    /// Base Event interface
    class Event {
        public:
            virtual ~Event() {}
            /// Each event must have event type assign, use VILLAIN_EVENT_TYPE(EventClass) macro to simplify generating this
            virtual EventType getType() const = 0;
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
        using EventCallback = std::function<void(Event& event)>;

        public:
            /// Call once for every new listener we want to subscribe
            void registerListener(EventListener* listener) {
                listeners.push_back(listener);
            }
            /// Stop sending events to this listener
            void unregisterListener(EventListener* listener) {
                listeners.erase(std::remove(listeners.begin(), listeners.end(), listener), listeners.end());
            }
            /// Call once for every new event callback function we want to subscribe
            void registerCallback(const EventCallback& callback) {
                callbacks.push_back(callback);
            }
            /// Stop sending events to this function callback
            void unregisterCallback(const EventCallback& callback) {
                callbacks.erase(std::remove_if(callbacks.begin(), callbacks.end(),
                            [&](const EventCallback& cb) {
                            return cb.target<EventCallback>() == callback.target<EventCallback>();
                            }), callbacks.end());
            }

            /// Must be called every time we want to dispatch an event
            void dispatchEvent(Event& event) {
                for (auto listener : listeners) {
                    listener->handleEvent(event);
                }

                for (auto callback : callbacks) {
                    callback(event);
                }
            }

            /// Get all registered listeners
            std::vector<EventListener*>& getListeners() { return listeners; }

        private:
            std::vector<EventListener*> listeners;
            std::vector<EventCallback> callbacks;
    };
}

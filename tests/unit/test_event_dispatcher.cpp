#include "catch2/catch_approx.hpp"
#include "catch2/catch_test_macros.hpp"

#include "EventDispatcher.hpp"

// Events provided by engine
#include "events/KeyboardEvent.hpp"
#include "events/MouseEvent.hpp"
#include "events/WindowResizeEvent.hpp"

class MockEventListener : public Villain::EventListener {
    public:
        virtual void handleEvent(Villain::Event &event) override {
            handledEvents.push_back(&event);
        }
        bool hasHandledEvent(Villain::Event& event) {
            return std::find(handledEvents.begin(), handledEvents.end(), &event) != handledEvents.end();
        }

    private:
        std::vector<Villain::Event*> handledEvents;
};

TEST_CASE("EventDispatcher Test", "[Event System]") {
    Villain::EventDispatcher dispatcher;
    MockEventListener listener1;
    MockEventListener listener2;

    SECTION("Register and Unregister Listeners") {
        dispatcher.registerListener(&listener1);
        dispatcher.registerListener(&listener2);

        REQUIRE(dispatcher.getListeners().size() == 2);

        dispatcher.unregisterListener(&listener1);

        REQUIRE(dispatcher.getListeners().size() == 1);
    }

    SECTION("Dispatch Event") {
        Villain::WindowResizeEvent event(800, 600);

        dispatcher.registerListener(&listener1);
        dispatcher.registerListener(&listener2);

        dispatcher.dispatchEvent(event);

        REQUIRE(listener1.hasHandledEvent(event));
        REQUIRE(listener2.hasHandledEvent(event));
    }
}

TEST_CASE("KeyboardEvent Test", "[Event Sytem]") {
    SECTION("Test getters") {
        Villain::KeyboardEvent event(Villain::KeyEventType::PRESS, Villain::KeyCode::w);
        Villain::KeyboardEvent event2(Villain::KeyEventType::RELEASE, Villain::KeyCode::w);

        REQUIRE(event.getKey() == Villain::KeyCode::w);
        REQUIRE(event.isPressed());
        REQUIRE_FALSE(event2.isPressed());
    }
}

TEST_CASE("MouseEvent Test", "[Event Sytem]") {
    SECTION("Test getters") {
        Villain::MouseEvent moveEvent(Villain::MouseEventType::MOVE, {100.0f, 100.0f}, {1.0f, 1.0f});
        Villain::MouseEvent buttonEvent(Villain::MouseEventType::CLICK, Villain::MouseButton::MIDDLE);
        Villain::MouseEvent wheelEvent(Villain::MouseEventType::SCROLL, 1);

        REQUIRE(moveEvent.getMousePos() == glm::vec2(100.0f, 100.0f));
        REQUIRE(moveEvent.getMouseOffset() == glm::vec2(1.0f, 1.0f));

        REQUIRE(buttonEvent.getButton() == Villain::MouseButton::MIDDLE);
        REQUIRE_FALSE(buttonEvent.isLeftMouseButton());
        REQUIRE(buttonEvent.isMiddleMouseButton());
        REQUIRE_FALSE(buttonEvent.isRightMouseButton());

        REQUIRE(wheelEvent.getMouseWheelDelta() == 1);
    }
}

TEST_CASE("WindowResizeEvent Test", "[Event Sytem]") {
    SECTION("Get Width and Height") {
        int width = 800;
        int height = 600;
        Villain::WindowResizeEvent event(width, height);

        REQUIRE(event.getWidth() == width);
        REQUIRE(event.getHeight() == height);
    }
}

#include "ScriptComponent.hpp"

#include "Engine.hpp"
#include "LuaBindings.hpp"

#include "events/KeyboardEvent.hpp"

namespace Villain {
    ScriptComponent::ScriptComponent(const std::string& scriptFilename): script(scriptFilename) {
        VILLAIN_SET_COMPONENT_ID(ScriptComponent);

        LuaBindings::registerBindings(script.getLuaState());
        script.open();
    }

    ScriptComponent::~ScriptComponent() {
        getParent()->getEngine()->getEventDispatcher()->unregisterCallback(BIND_EVENT_FN(onEvent));
    }

    void ScriptComponent::update(float deltaTime) {
        lua_getglobal(script.getLuaState(), "Update"); // Get the Lua function by name.

        // Push the 1st argument: delta time number
        lua_pushnumber(script.getLuaState(), deltaTime);

        // Push the 2nd argument:userdata for the SceneNode while setting metatable for it
        lua_pushlightuserdata(script.getLuaState(), getParent());
        luaL_setmetatable(script.getLuaState(), "SceneNode");

        // Call the Lua function with two arguments (deltaTime and the scene node pointer).
        if (lua_pcall(script.getLuaState(), 2, 0, 0) != LUA_OK) {
            // Handle any errors here.
            std::string errorMessage = lua_tostring(script.getLuaState(), -1);
            VILLAIN_CRIT("Lua error in update() for {}: {}", getParent()->getName(), errorMessage);
        }
    }

    void ScriptComponent::addToEngine(Engine* engine) {
        getParent()->getEngine()->getEventDispatcher()->registerCallback(BIND_EVENT_FN(onEvent));
    }

    void ScriptComponent::onEvent(Event& event) {
        if (event.getType() == EventType::KeyboardEvent) {
            lua_getglobal(script.getLuaState(), "OnKey");

            // Event callbacks in lua are optional
            if (lua_isfunction(script.getLuaState(), -1)) {
                KeyboardEvent e = static_cast<KeyboardEvent&>(event);
                lua_pushnumber(script.getLuaState(), (char)e.getKey());
                lua_pushlightuserdata(script.getLuaState(), getParent());
                luaL_setmetatable(script.getLuaState(), "SceneNode");

                if (lua_pcall(script.getLuaState(), 2, 0, 0) != LUA_OK) {
                    std::string errorMessage = lua_tostring(script.getLuaState(), -1);
                    VILLAIN_CRIT("Lua error in onKey() for {}: {}", getParent()->getName(), errorMessage);
                }
            }

            // TODO: other callbacks with appropriate params
        }
    }
}

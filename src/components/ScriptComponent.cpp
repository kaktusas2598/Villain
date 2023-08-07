#include "ScriptComponent.hpp"

#include "Engine.hpp"
#include "script/LuaBindings.hpp"

#include "events/CollisionEvent.hpp"
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
            VILLAIN_CRIT("Lua error in Update() for {}: {}", getParent()->getName(), errorMessage);
        }

        ///////////////////////////////
        // Update manipulators
        for (auto& m: newManipulators) {
            manipulators.push_back(std::move(m));
        }
        newManipulators.clear();

        for (auto& m: manipulators) {
            if (m->update(deltaTime)) {
                // Manipulation complete, resume coroutine
                issueNextTask();
            }
        }

        // Remove completed manipulations
        manipulators.erase(
                std::remove_if(manipulators.begin(), manipulators.end(),
                    [](const std::shared_ptr<Manipulator>& m)
                    { return m->complete; } ), manipulators.end()
                );
        ///////////////////////////////
    }

    void ScriptComponent::addToEngine(Engine* engine) {
        getParent()->getEngine()->getEventDispatcher()->registerCallback(BIND_EVENT_FN(onEvent));

        lua_getglobal(script.getLuaState(), "Init");
        if (lua_isfunction(script.getLuaState(), -1)) {
            lua_pushlightuserdata(script.getLuaState(), getParent());
            luaL_setmetatable(script.getLuaState(), "SceneNode");

            if (lua_pcall(script.getLuaState(), 1, 0, 0) != LUA_OK) {
                // Handle any errors here.
                std::string errorMessage = lua_tostring(script.getLuaState(), -1);
                VILLAIN_CRIT("Lua error in Init() for {}: {}", getParent()->getName(), errorMessage);
            }
        }
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
                    VILLAIN_CRIT("Lua error in OnKey() for {}: {}", getParent()->getName(), errorMessage);
                }
            }
        }

        if (event.getType() == EventType::CollisionEvent) {
            CollisionEvent collisionEvent = static_cast<CollisionEvent&>(event);
            // Check if it was dispatched from the same node RigidBodyComponent
            if (collisionEvent.isVerified()) {
                lua_getglobal(script.getLuaState(), "OnCollide");
                if (lua_isfunction(script.getLuaState(), -1)) {
                    lua_pushlightuserdata(script.getLuaState(), getParent());
                    luaL_setmetatable(script.getLuaState(), "SceneNode");

                    lua_pushlightuserdata(script.getLuaState(), collisionEvent.getBody1());
                    luaL_setmetatable(script.getLuaState(), "RigidBody");

                    lua_pushlightuserdata(script.getLuaState(), collisionEvent.getBody2());
                    luaL_setmetatable(script.getLuaState(), "RigidBody");

                    if (lua_pcall(script.getLuaState(), 3, 0, 0) != LUA_OK) {
                        std::string errorMessage = lua_tostring(script.getLuaState(), -1);
                        VILLAIN_CRIT("Lua error in OnCollide() for {}: {}", getParent()->getName(), errorMessage);
                    }
                }
            }
        }
        // TODO: other callbacks with appropriate params
    }

    void ScriptComponent::issueNextTask() {
        lua_getglobal(script.getLuaState(), "IssueNextTask");
        if (lua_isfunction(script.getLuaState(), -1)) {
            lua_pushlightuserdata(script.getLuaState(), getParent());
            if (lua_pcall(script.getLuaState(), 1, 0, 0) != LUA_OK) {
                std::string errorMessage = lua_tostring(script.getLuaState(), -1);
                VILLAIN_CRIT("Lua error in IssueNextTask() for {}: {}", getParent()->getName(), errorMessage);
            }
        }
    }
}

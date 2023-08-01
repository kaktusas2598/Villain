#include "ScriptComponent.hpp"

namespace Villain {
    ScriptComponent::ScriptComponent(const std::string& scriptFilename): script(scriptFilename) {
        LuaBindings::registerBindings(script.getLuaState());
        script.open();
    }

    void ScriptComponent::update(float deltaTime) {
        lua_getglobal(script.getLuaState(), "update"); // Get the Lua function by name.

        lua_pushnumber(script.getLuaState(), deltaTime);
        lua_pushlightuserdata(script.getLuaState(), getParent());

        // Call the Lua function with two arguments (deltaTime and the scene node pointer).
        if (lua_pcall(script.getLuaState(), 2, 0, 0) != LUA_OK) {
            // Handle any errors here.
            std::string errorMessage = lua_tostring(script.getLuaState(), -1);
            VILLAIN_CRIT("Lua error in update script for {}: {}", getParent()->getName(), errorMessage);
        }
    }
}

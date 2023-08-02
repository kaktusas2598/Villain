#include "ScriptComponent.hpp"

namespace Villain {
    ScriptComponent::ScriptComponent(const std::string& scriptFilename): script(scriptFilename) {
        VILLAIN_SET_COMPONENT_ID(ScriptComponent);

        LuaBindings::registerBindings(script.getLuaState());
        script.open();
    }

    void ScriptComponent::update(float deltaTime) {
        lua_getglobal(script.getLuaState(), "update"); // Get the Lua function by name.

        // Push the 1st argument: delta time number
        lua_pushnumber(script.getLuaState(), deltaTime);

        // Push the 2nd argument:userdata for the SceneNode while setting metatable for it
        lua_pushlightuserdata(script.getLuaState(), getParent());
        luaL_setmetatable(script.getLuaState(), "SceneNode");

        // Call the Lua function with two arguments (deltaTime and the scene node pointer).
        if (lua_pcall(script.getLuaState(), 2, 0, 0) != LUA_OK) {
            // Handle any errors here.
            std::string errorMessage = lua_tostring(script.getLuaState(), -1);
            VILLAIN_CRIT("Lua error in update script for {}: {}", getParent()->getName(), errorMessage);
        }
    }
}

#include "LuaBindings.hpp"

#include "Engine.hpp"
#include "SoundManager.hpp"

namespace Villain {

    void LuaBindings::registerBindings(lua_State* L) {
        // Register C++ -> Lua wrappers
        // NOTE: Must be done before running lua script
        lua_register(L, "playSound", lua_playSound);
        lua_register(L, "playMusic", lua_playMusic);
        lua_register(L, "getScreenWidth", lua_getScreenWidth);
        lua_register(L, "getScreenHeight", lua_getScreenHeight);
        lua_register(L, "quit", lua_quit);

        createSceneNodeMetatable(L);
    }

    void LuaBindings::createSceneNodeMetatable(lua_State* L) {
        // Set methods for the metatable
        luaL_Reg sceneNodeMethods[] = {
            { "getName", lua_SceneNode_getName },
            // Add more methods as needed
            { NULL, NULL } // The array must be null-terminated
        };

        if (luaL_newmetatable(L, "SceneNode")) {
            luaL_setfuncs(L, sceneNodeMethods, 0);

            // Set "__index" to point to the metatable itself so that we can access methods
            // from the metatable directly on userdata
            lua_pushvalue(L, -1);
            lua_setfield(L, -2, "__index");
        }
    }

    int LuaBindings::lua_SceneNode_getName(lua_State *L) {
        SceneNode* node = static_cast<SceneNode*>(lua_touserdata(L, 1));
        lua_pushstring(L, node->getName().c_str());
        return 1;
    }

    int LuaBindings::lua_playSound(lua_State *L) {
        std::string soundID = (std::string)lua_tostring(L, 1);
        SoundManager::Instance()->playSound(soundID, 0);
        return 0;
    }

    int LuaBindings::lua_playMusic(lua_State *L) {
        std::string soundID = (std::string)lua_tostring(L, 1);
        SoundManager::Instance()->playMusic(soundID, 0);
        return 0;
    }

    int LuaBindings::lua_getScreenWidth(lua_State *L) {
        int screenWidth = Engine::getScreenWidth();
        lua_pushnumber(L, screenWidth);
        return 1;
    }

    int LuaBindings::lua_getScreenHeight(lua_State *L) {
        int screenHeight = Engine::getScreenHeight();
        lua_pushnumber(L, screenHeight);
        return 1;
    }

    int LuaBindings::lua_quit(lua_State *L) {
        Engine::setRunning(false);
        return 0;
    }
}

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
        lua_register(L, "addLog", lua_addLog);
        lua_register(L, "quit", lua_quit);

        createSceneNodeMetatable(L);
    }

    void LuaBindings::createSceneNodeMetatable(lua_State* L) {
        // Set methods for the metatable
        luaL_Reg sceneNodeMethods[] = {
            { "getName", lua_SceneNode_getName },
            { "getComponent", lua_SceneNode_getComponent },
            { "setPosition", lua_SceneNode_setPosition },
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

    int LuaBindings::lua_SceneNode_getComponent(lua_State *L) {
        SceneNode* node = static_cast<SceneNode*>(lua_touserdata(L, 1));
        const char* componentName = luaL_checkstring(L, 2);

        if (strcmp(componentName, "Camera") == 0) {
            Camera* camera = node->getComponent<Camera>();
            if (camera) {
                // TODO: decide how we will represent this: userdata, lightuserdata?
                lua_pushboolean(L, true);
                return 1;
            }
        }

        VILLAIN_WARN("Lua error in getComponent(): component \"{}\" not found", componentName);
        lua_pushnil(L);
        return 1;
    }

    int LuaBindings::lua_SceneNode_setPosition(lua_State *L) {
        // Check if the number of arguments is correct (expected 4: self + 3 for x, y, z)
        int numArgs = lua_gettop(L);
        if (numArgs != 4) {
            luaL_error(L, "Invalid number of arguments. Expected 3 (x, y, z).");
            return 0;
        }

        SceneNode* node = static_cast<SceneNode*>(lua_touserdata(L, 1));
        float x = static_cast<float>(luaL_checknumber(L, 2));
        float y = static_cast<float>(luaL_checknumber(L, 3));
        float z = static_cast<float>(luaL_checknumber(L, 4));

        node->getTransform()->setPos({x, y, z});

        return 0;
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

    int LuaBindings::lua_addLog(lua_State *L) {
        int nArgs = lua_gettop(L);
        // TODO: support building variadic args
        for (int i = 0; i < nArgs; i++) {
            if (lua_isstring(L, i)) {
                DebugConsole::Instance()->addLog(lua_tostring(L,i));
            }
        }
        return 0;
    }

    int LuaBindings::lua_quit(lua_State *L) {
        Engine::setRunning(false);
        return 0;
    }
}

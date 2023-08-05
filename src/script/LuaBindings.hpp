#pragma once

#include <glm/glm.hpp>

extern "C" {
# include "lua.h"
# include "lauxlib.h"
# include "lualib.h"
}

namespace Villain {

    class LuaBindings {
        public:
            /// Must be called before executing script using these bindings
            static void registerBindings(lua_State* L);

            /// Convert glm::vec3 to lua table
            static void pushVec3ToLua(lua_State* L, const glm::vec3& vec);
            /// Convert lua table to glm::vec3
            static glm::vec3 readVec3FromLua(lua_State* L, int index);
        private:
            static void createSceneNodeMetatable(lua_State* L);

            /// Core Bindings
            static int lua_getAudio(lua_State *L);
            static int lua_getScreenWidth(lua_State *L);
            static int lua_getScreenHeight(lua_State *L);
            static int lua_addLog(lua_State *L);
            static int lua_keyCode(lua_State *L);
            static int lua_quit(lua_State *L);
            /// Check if userdata is nil
            static int lua_isNil(lua_State* L);

            /// SceneNode metatable bindings
            static int lua_SceneNode_getName(lua_State *L);
            static int lua_SceneNode_getComponent(lua_State *L);
            static int lua_SceneNode_getPosition(lua_State *L);
            static int lua_SceneNode_setPosition(lua_State *L);
    };
}

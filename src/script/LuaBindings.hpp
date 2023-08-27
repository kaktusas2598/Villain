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
            static int lua_GetAudio(lua_State *L);
            static int lua_GetMouseCoords(lua_State *L);
            static int lua_GetMouseOffsets(lua_State *L);
            static int lua_GetScreenWidth(lua_State *L);
            static int lua_GetScreenHeight(lua_State *L);
            static int lua_AddLog(lua_State *L);
            static int lua_KeyCode(lua_State *L);
            static int lua_IsKeyDown(lua_State *L);
            static int lua_IsKeyPressed(lua_State *L);
            static int lua_Quit(lua_State *L);
            static int lua_SetHDRMap(lua_State *L);
            /// Check if userdata is nil
            static int lua_IsNil(lua_State* L);

            /// SceneNode metatable bindings
            static int lua_SceneNode_getName(lua_State *L);
            static int lua_SceneNode_getComponent(lua_State *L);
            static int lua_SceneNode_getPosition(lua_State *L);
            static int lua_SceneNode_setPosition(lua_State *L);
            static int lua_SceneNode_addMoveBehaviour(lua_State *L);
            static int lua_SceneNode_addCurveMoveBehaviour(lua_State *L);
            static int lua_SceneNode_addTeleportBehaviour(lua_State *L);
    };
}

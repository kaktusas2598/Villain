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

        private:
            static void createAudioSourceMetatable(lua_State* L);
            static void createSceneNodeMetatable(lua_State* L);

            /// Core Bindings
            static int lua_getAudio(lua_State *L);
            static int lua_getScreenWidth(lua_State *L);
            static int lua_getScreenHeight(lua_State *L);
            static int lua_addLog(lua_State *L);
            static int lua_keyCode(lua_State *L);
            static int lua_quit(lua_State *L);

            /// Audio Source Bindings
            static int lua_AudioSource_isPlaying(lua_State* L);
            static int lua_AudioSource_play(lua_State* L);
            static int lua_AudioSource_pause(lua_State* L);
            static int lua_AudioSource_stop(lua_State* L);
            static int lua_AudioSource_rewind(lua_State* L);
            static int lua_AudioSource_setVolume(lua_State* L);
            static int lua_AudioSource_setPitch(lua_State* L);
            static int lua_AudioSource_setLooping(lua_State* L);
            static int lua_AudioSource_setPositions(lua_State* L);
            static int lua_AudioSource_setVelocity(lua_State* L);

            /// SceneNode metatable bindings
            static int lua_SceneNode_getName(lua_State *L);
            static int lua_SceneNode_getComponent(lua_State *L);
            static int lua_SceneNode_getPosition(lua_State *L);
            static int lua_SceneNode_setPosition(lua_State *L);

            /// Convert glm::vec3 to lua table
            static void pushVec3ToLua(lua_State* L, const glm::vec3& vec);
            /// Convert lua table to glm::vec3
            static glm::vec3 readVec3FromLua(lua_State* L, int index);
    };
}

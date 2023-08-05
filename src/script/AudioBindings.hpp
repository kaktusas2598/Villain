#pragma once

#include <glm/glm.hpp>

extern "C" {
# include "lua.h"
# include "lauxlib.h"
# include "lualib.h"
}

namespace Villain {

    class AudioBindings {
        public:
            /// Must be called before executing script using these bindings
            static void registerBindings(lua_State* L);

        private:
            /// AudioSource metatable bindings
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
    };
}

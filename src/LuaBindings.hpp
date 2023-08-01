#pragma once

extern "C" {
# include "lua.h"
# include "lauxlib.h"
# include "lualib.h"
}

namespace Villain {

    // TODO:1. implement way more bindings
    // 2. create metatables
    class LuaBindings {
        public:
            /// Must be called before executing script using these bindings
            static void registerBindings(lua_State* L);

            static int lua_playSound(lua_State *L);
            static int lua_playMusic(lua_State *L);

            static int lua_getScreenWidth(lua_State *L);
            static int lua_getScreenHeight(lua_State *L);

            static int lua_quit(lua_State *L);
    };

}

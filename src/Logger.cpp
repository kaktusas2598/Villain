#include "Logger.hpp"

namespace Villain {
    Logger* Logger::s_pInstance = nullptr;

    void Logger::dumpStack(lua_State* L) {
        spdlog::info("---------------------");

        int top = lua_gettop(L);
        for (int i = 1; i <= top; i++) {
            spdlog::info("{}\t{}", i, luaL_typename(L, i));

            switch (lua_type(L, i)) {
                case LUA_TNUMBER:
                    spdlog::info("{}", lua_tonumber(L, i));
                    break;
                case LUA_TSTRING:
                    spdlog::info("{}", lua_tostring(L, i));
                    break;
                case LUA_TBOOLEAN:
                    spdlog::info("{}", lua_toboolean(L, i) ? "true" : "false");
                    break;
                case LUA_TNIL:
                    spdlog::info("nil");
                    break;
                default:
                    spdlog::info("{}", lua_topointer(L, i));
                    break;
            }
        }

        spdlog::info("---------------------");
    }
}

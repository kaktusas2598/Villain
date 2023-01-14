#include "Logger.hpp"

namespace Villain {
    Logger* Logger::s_pInstance = nullptr;

    void Logger::dumpStack (lua_State *L) {
        int top=lua_gettop(L);
        logFile << "---------------------\n" << std::endl;
        for (int i=1; i <= top; i++) {
            logFile << i << "\t" << luaL_typename(L,i) << std::endl;
            switch (lua_type(L, i)) {
            case LUA_TNUMBER:
                logFile << lua_tonumber(L,i) << std::endl;
                break;
            case LUA_TSTRING:
                logFile << lua_tostring(L,i) << std::endl;
                break;
            case LUA_TBOOLEAN:
                logFile << (lua_toboolean(L, i) ? "true" : "false") << std::endl;
                break;
            case LUA_TNIL:
                logFile << "nil" << std::endl;
                break;
            default:
                logFile << lua_topointer(L,i) << std::endl;
            break;
            }
        }
        logFile << "---------------------\n" << std::endl;
    }
}

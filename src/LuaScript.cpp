#include "LuaScript.hpp"

namespace Villain {

    LuaScript::LuaScript(const std::string& fileName): fileName(fileName) {
        L = luaL_newstate();

        if (L)
            luaL_openlibs(L);
    }

    LuaScript::~LuaScript() {
        Logger::Instance()->info("Closing Lua state. Script: {}", fileName);
        if (L) {
            lua_close(L);
        }
    }

    void LuaScript::open() {
        if (luaL_loadfile(L, fileName.c_str()) || lua_pcall(L, 0, 0, 0)) {
            std::string errorMessage = lua_tostring(L, -1);
            exitWithError("Could not load script: " + fileName + ". " + errorMessage);
            L = 0;
        }
    }

    void LuaScript::close() {
        Logger::Instance()->info("Closing Lua state.");
        if (L) {
            lua_close(L);
        }
    }


    void LuaScript::printError(const std::string& variableName, const std::string& reason) {
        //TODO: handle this better
        Logger::Instance()->error(reason.c_str());
        exitWithError(reason);
    }

    // TODO: add methods for string and float arrays
    std::vector<int> LuaScript::getIntVector(const std::string& name) {
        std::vector<int> v;
        // First check if variable is defined in global table
        lua_getglobal(L, name.c_str());
        if (lua_isnil(L, -1)) {
            return std::vector<int>();
        }

        lua_pushnil(L);
        while (lua_next(L, -2)) {
            v.push_back((int) lua_tonumber(L, -1));
            lua_pop(L, 1);
        }
        clean();
        return v;
    }

    // Only works for global tables at the
    std::vector<std::string> LuaScript::getTableKeys(const std::string& name) {
        // Lua function for getting table keys, will return "key1, key2, etc"
        std::string code =
        "function getKeys(name) "
        "s = \"\""
        "for k, v in pairs(_G[name]) do "
        "    s = s..k..\",\" "
        "    end "
        "return s "
        "end"; // function for getting table keys
        luaL_loadstring(L, code.c_str());
        lua_pcall(L, 0, 0, 0);
        lua_getglobal(L, "getKeys");
        lua_pushstring(L, name.c_str());
        lua_pcall(L, 1, 1, 0);

        std::string luaReturn = lua_tostring(L, -1);
        std::vector<std::string> tableKeys;
        std::string temp = "";

        for(unsigned int i = 0; i < luaReturn.size(); i++) {
            if (luaReturn.at(i) != ',') {
                temp += luaReturn.at(i);
            } else {
                tableKeys.push_back(temp);
                temp = "";
            }
        }
        clean();
        return tableKeys;
    }

}

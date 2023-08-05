#pragma once

#include <string>
#include <vector>
#include <iostream>

// Lua is written in C, so compiler needs to know how to link its libraries
extern "C" {
# include "lua.h"
# include "lauxlib.h"
# include "lualib.h"
}

#include "ErrorHandler.hpp"
#include "Logger.hpp"

/*
 * To ensure that we're not reading undefined variables from Lua state we must make sure to always close scripts when done:
 * 1. Open script luaL_dofile(L, filename)
 * 2. Load Data
 * 3. Close script lua_close(L)
 */
namespace Villain {
    class LuaScript {
        public:
            LuaScript(const std::string& filename);
            ~LuaScript();

            // Loads and runs Lua script, make sure to register C++ functions before running this
            void open();
            // Closes script
            void close();

            void printError(const std::string& variableName, const std::string& reason);
            std::vector<int> getIntVector(const std::string& name);
            std::vector<std::string> getTableKeys(const std::string& name);

            // Clean Lua stack
            void clean() {
                int n = lua_gettop(L);
                lua_pop(L, n);
            }

            template<typename T>
            T get(const std::string& variableName) {
                if (!L) {
                    printError(variableName, "Script was not loaded");
                    return lua_getdefault<T>();
                }

                T result;
                // Variable is on top of the stack
                if (lua_gettostack(variableName)) {
                    result = lua_get<T>(variableName);
                } else {
                    result = lua_getdefault<T>();
                }

                // Pop existing items from the stack
                lua_pop(L, level + 1);
                return result;
            }

            // Checks if variable is defined
            bool lua_gettostack(const std::string& variableName) {
                level = 0;
                std::string var = "";
                for (unsigned int i = 0; i < variableName.size(); i++) {
                    if (variableName.at(i) == '.') {
                        if (level == 0) {
                            lua_getglobal(L, var.c_str());
                        } else {
                            lua_getfield(L, -1, var.c_str());
                        }

                        if (lua_isnil(L, -1)) {
                            printError(variableName, var + " is not defined");
                            return false;
                        } else {
                            var = "";
                            level++;
                        }
                    } else {
                        var += variableName.at(i);
                    }
                }

                if (level == 0) {
                    lua_getglobal(L, var.c_str());
                } else {
                    lua_getfield(L, -1, var.c_str());
                }
                if (lua_isnil(L, -1)) {
                    printError(variableName, var + " is not defined");
                    return false;
                }

                return true;
            }

            // Generic getter
            template<typename T>
            T lua_get(const std::string& variableName) {
                return 0;
            }

            // Generic default value getter for a lua variable
            template<typename T>
            T lua_getdefault() {
                return 0;
            }

            template<typename T>
            std::vector<T> getArray(const std::string& name) {
                std::vector<T> v;
                if(!lua_gettostack(name)) {
                    exitWithError("Lua: Array not found");
                    clean();
                    return std::vector<T>();
                }

                lua_pushnil(L);
                while (lua_next(L, -2)) {
                    // -1 means top of the Lua stack
                    if(lua_isnumber(L, -1)) {
                        v.push_back((int) lua_tonumber(L, -1));
                    }
                    lua_pop(L, 1);
                }
                clean();
                return v;
            }

            lua_State* getLuaState() { return L; }
            std::string getFilename() const { return fileName; }
        private:
            bool luaOk(lua_State* L, int call) {
                if (call != LUA_OK) {
                    std::string errorMessage = lua_tostring(L, -1);
                    exitWithError(errorMessage);
                    return false;
                }

                return true;
            }

            lua_State* L;
            int level;
            std::string fileName;
    };

    // Template specializations

    template <>
    inline bool LuaScript::lua_get(const std::string& variableName) {
        return (bool) lua_toboolean(L, -1);
    }

    template <>
    inline float LuaScript::lua_get(const std::string& variableName) {
        if (!lua_isnumber(L, -1)) {
            printError(variableName, "Lua: not a float");
        }
        return (float) lua_tonumber(L, -1);
    }

    template <>
    inline int LuaScript::lua_get(const std::string& variableName) {
        if (!lua_isnumber(L, -1)) {
            printError(variableName, "Lua: not an integer");
        }
        return (int) lua_tonumber(L, -1);
    }

    template <>
    inline std::string LuaScript::lua_get(const std::string& variableName) {
        std::string s = "null";
        if (lua_isstring(L, -1)) {
            s = std::string(lua_tostring(L, -1));
        } else {
            printError(variableName, "Lua: not a string");
        }
        return s;
    }

    template <>
    inline std::string LuaScript::lua_getdefault<std::string>() {
        return "null";
    }

    template<>
    inline std::vector<std::string> LuaScript::getArray(const std::string& name) {
        std::vector<std::string> v;
        if(!lua_gettostack(name)) {
            exitWithError("Lua: Array not found");
            clean();
            return std::vector<std::string>();
        }

        lua_pushnil(L);
        while (lua_next(L, -2)) {
            if (lua_isstring(L, -1)) {
                v.push_back((std::string) lua_tostring(L, -1));
            }
            lua_pop(L, 1);
        }
        clean();
        return v;
    }
}

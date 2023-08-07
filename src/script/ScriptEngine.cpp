#include "ScriptEngine.hpp"

#include "Logger.hpp"
#include "LuaBindings.hpp"

#include <cassert>
#include <iostream>

namespace Villain {

    ScriptEngine* ScriptEngine::s_pInstance = nullptr;

    ScriptEngine::ScriptEngine() {
        build();
    }

    void ScriptEngine::build() {
        state = luaL_newstate();
        if (state)
            luaL_openlibs(state);

        LuaBindings::registerBindings(state);
    }

    void ScriptEngine::init(std::string fileName) {
        script = new LuaScript(fileName);
        state = script->getLuaState();

        build();

        script->open();
    }

    void ScriptEngine::loadScript(std::string fileName) {
        if (luaL_loadfile(state, fileName.c_str()) || lua_pcall(state, 0, 0, 0)) {
            std::string errorMessage = lua_tostring(state, -1);
            std::cout << errorMessage << std::endl;
            Logger::Instance()->error(errorMessage.c_str());
        }
    }

    void ScriptEngine::loadChunk(const char* chunk) {
        if (luaL_loadstring(state, chunk) || lua_pcall(state, 0, 0, 0)) {
            std::string errorMessage = lua_tostring(state, -1);
            std::cout << errorMessage << std::endl;
            Logger::Instance()->error(errorMessage.c_str());
        }
    }

    // FIXME: Issue with this functions is that it will only return
    // errors or values returned with "return", we could redirect stdout from lua
    // to buffer using std::cout.rdbuf()!!!
    std::string ScriptEngine::runChunk(const char* chunk) {
        // Clean Lua stack before running another chunk
        int n = lua_gettop(state);
        lua_pop(state, n);

        if (luaL_dostring (state, chunk)) {
            std::string errorMessage = lua_tostring(state, -1);
            return errorMessage;
        }
        // Else if no error check for other output
        if (lua_isstring(state, -1)) {
            std::string output = lua_tostring(state, -1);
            return output;
        } else {
            VILLAIN_CRIT("Non String!");
        }
        return std::string();
    }
}

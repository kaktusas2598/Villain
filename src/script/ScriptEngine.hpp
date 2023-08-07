#pragma once

#include "LuaScript.hpp"
#include "Engine.hpp"
#include <string>

namespace Villain {

    class ScriptEngine {
        public:
            ScriptEngine();

            static ScriptEngine* Instance() {
                if(s_pInstance == 0) {
                    s_pInstance = new ScriptEngine();
                    return s_pInstance;
                }
                return s_pInstance;
            }

            // Setup metatables and bind c++ methods
            void build();

            // Setup metatables and bind c++ methods, load script
            // Tried loading init script for all states, functions can be shared, but variables
            // don't seem to be shared even in the same lua_state?
            void init(std::string fileName);

            // Loads and runs script
            void loadScript(std::string fileName);
            void loadChunk(const char* chunk);
            std::string runChunk(const char* chunk); // Identical as loadChunk(), but returns any output

            // Close all scripts
            void close() {
                script->close();
            }

            /// Temporary place, this method should go to dedicated scripting classes
            bool luaOk(lua_State* L, int call) {
                if (call != LUA_OK) {
                    std::string errorMessage = lua_tostring(L, -1);
                    std::cout << errorMessage << std::endl;
                    Logger::Instance()->error(errorMessage.c_str());
                    return false;
                }
                return true;
            }

        private:
            static ScriptEngine* s_pInstance;

            lua_State* state;
            // Used for init script and other scripts will load onto same state
            LuaScript* script;
            std::string fileName;
            // This identifies which table to get from lua ascript, dont like this for some reason
            std::string id;
    };
}

#pragma once

#include "LuaBindings.hpp"
#include "LuaScript.hpp"
#include "NodeComponent.hpp"

namespace Villain {

    class ScriptComponent : public NodeComponent {
        public:
            ScriptComponent(const std::string& scriptFilename): script(scriptFilename) {
                LuaBindings::registerBindings(script.getLuaState());
            }
            ~ScriptComponent() {}

            virtual void update(float deltaTime) override {
                // TODO: Instead calling scritp, call specific function from the script like 'update' ?
                //To call a Lua function named "update" defined in the Lua script:
                // lua_getglobal(luaScript.getLuaState(), "update");
                // lua_pushnumber(luaScript.getLuaState(), deltaTime);
                // lua_pcall(luaScript.getLuaState(), 1, 0, 0);
                script.open();
            }

        private:
            LuaScript script;
    };
}

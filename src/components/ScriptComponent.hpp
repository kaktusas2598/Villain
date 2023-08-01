#pragma once

#include "LuaBindings.hpp"
#include "LuaScript.hpp"
#include "NodeComponent.hpp"

namespace Villain {

    /// Allows attaching custom Lua logic to a node in scene graph
    class ScriptComponent : public NodeComponent {
        public:
            ScriptComponent(const std::string& scriptFilename);
            ~ScriptComponent() {}

            /// Calls Lua "update" function
            virtual void update(float deltaTime) override;

        private:
            LuaScript script;
    };
}

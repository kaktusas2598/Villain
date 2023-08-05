#pragma once

#include "EventDispatcher.hpp"
#include "script/LuaScript.hpp"
#include "NodeComponent.hpp"

namespace Villain {

    /*! \brief Allows attaching Lua script to a node in scene graph
     *
     * Script can be executed every frame in Lua 'update' function or also respond to events
     * with optional Lua event callbacks
     */
    class ScriptComponent : public NodeComponent {
        public:
            ScriptComponent(const std::string& scriptFilename);
            ~ScriptComponent();

            /// Calls Lua "update" function
            virtual void update(float deltaTime) override;

            virtual void addToEngine(Engine* engine) override;

            /// Forwards Engine events to optional lua event callbacks defined by user
            void onEvent(Event& event);

            LuaScript& getScript() { return script; }
        private:
            LuaScript script;
    };
}

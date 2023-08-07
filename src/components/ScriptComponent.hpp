#pragma once

#include "EventDispatcher.hpp"
#include "script/LuaScript.hpp"
#include "script/Manipulators.hpp"
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

            /// Automation behaviours: move
            void move(const glm::vec3& target, float time) {
                newManipulators.emplace_back(std::make_shared<MoveManipulator>(getParent(), target, time));
            }
            /// Automation behaviours: move along curve
            void curveMove(const glm::vec3& target, const glm::vec3& waypoint, float time) {
                newManipulators.emplace_back(std::make_shared<CurveMoveManipulator>(getParent(), waypoint, target, time));
            }
            /// Automation behaviours: instant teleport
            void teleport(const glm::vec3& target) {
                newManipulators.emplace_back(std::make_shared<TeleportManipulator>(getParent(), target));
            }

            LuaScript& getScript() { return script; }
        private:
            /// Issues next task for dynamic behaviours/coroutines
            void issueNextTask();

            LuaScript script;

            /// Optional manipulators for automation via coroutines
            std::vector<std::shared_ptr<Manipulator>> manipulators;
            // This is protect in case manipulators are added while looping
            std::vector<std::shared_ptr<Manipulator>> newManipulators;
    };
}

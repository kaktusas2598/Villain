#pragma once

#include "SceneNode.hpp"

namespace Villain {

    // TODO: Implement MORE Manipulators:
    //  A* pathfind to target
    //  Scale, Rotation manipulators
    //  Color, Fade manipulators
    //  RotateAround, FollowPath, LookAt
    //  Shake, Trigger, Audio manipulators

    /// Base Manipulator class for Dynamic Behaviours scriptable in Lua using coroutines
    struct Manipulator {
        SceneNode* node;

        virtual bool update(float deltaTime) = 0;
        bool complete = false;
    };

    /// Linearly interpolates position of SceneNode
    struct MoveManipulator : public Manipulator {
        glm::vec3 startPos;
        glm::vec3 targetPos;
        float completionTime = 0.0f;
        float timePassed = 0.0f;

        MoveManipulator(SceneNode* n, glm::vec3 target, float fTime) {
            node = n;
            startPos = n->getTransform()->getPos();
            targetPos = target;
            completionTime = fTime;
        }

        bool update(float elapsedTime) override {
            timePassed += elapsedTime;
            // Calculate new pos based on time using linear interpolation
            glm::vec3 newPos = targetPos - startPos;
            newPos *= (timePassed / completionTime);
            newPos += startPos;
            node->getTransform()->setPos(newPos);

            if (timePassed >= completionTime) {
                node->getTransform()->setPos(targetPos);
                complete = true;
            }

            return complete;
        }
    };

    /// Instant teleportation for SceneNode
    struct TeleportManipulator : public Manipulator {
        glm::vec3 targetPos;

        TeleportManipulator(SceneNode* n, const glm::vec3& target) {
            node = n;
            targetPos = target;
        }

        bool update(float elapsedTime) override {
            node->getTransform()->setPos(targetPos);
            complete = true;
            return complete;
        }
    };

    ///  Moves SceneNode along curve (bezier spline) - non linear interpolation
    struct CurveMoveManipulator : public Manipulator {
        glm::vec3 startPos;
        glm::vec3 waypointPos;
        glm::vec3 targetPos;
        float completionTime = 0.0f;
        float timePassed = 0.0f;

        CurveMoveManipulator(SceneNode* n, glm::vec3 waypoint, glm::vec3 target, float fTime) {
            node = n;
            startPos = n->getTransform()->getPos();
            waypointPos = waypoint;
            targetPos = target;
            completionTime = fTime;
        }

        bool update(float deltaTime) override {
            timePassed += deltaTime;
            float t = timePassed / completionTime;

            glm::vec3 newPos = ((1 - t) * (1 - t) * startPos) + (2.0f * (1 - t) * t * waypointPos) + (t * t * targetPos);
            node->getTransform()->setPos(newPos);

            if (timePassed >= completionTime) {
                node->getTransform()->setPos(targetPos);
                complete = true;
            }

            return complete;
        }

    };
}

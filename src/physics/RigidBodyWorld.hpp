#pragma once

#include "Contact.hpp"
#include "ForceGenerator.hpp"
#include "NarrowPhase.hpp"
#include "rendering/DebugRenderer.hpp"
#include <algorithm>
#include <functional>
#include <vector>

namespace Villain {

    /*! \brief Dynamic world of Rigid Bodies with collision
     *
     * Keeps track of rigid bodies and their collision shapes and provice means to simulate world
     */
    class RigidBodyWorld {
        public:
            typedef std::vector<RigidBody*> RigidBodies;
            typedef std::vector<CollisionPrimitive*> Colliders;

            /// If not iterations are provided, when it will be four times the number of contacts used
            RigidBodyWorld(Engine* e, unsigned maxContacts, unsigned iterations = 0);
            ~RigidBodyWorld();

            /// Cast a ray in rigid body world and callback to each result, by defaults to only the closest hit
            void cast(const glm::vec3& from, const glm::vec3& to, std::function<void(RayHitResult&)> callback, bool closestHitOnly = true);

            /// Calls all contact generators to report any contacts and returns numbers of generated contacts
            unsigned generateContacts();

            /// Main method for processing physics in rigid body world
            void runPhysics(float deltaTime);

            /// Initialises world for a simulation frame which clears all force accumulators for particles
            /// in this world, after calling this methods, forces can be added
            void startFrame();

            /// Integrate all bodies in this world forward in given delta time
            void integrate(float deltaTime);

            void debugDraw(Camera* camera);
            void setDebugDraw(bool debug) { debugDrawEnabled = debug; }

            void addBody(RigidBody* body) { bodies.push_back(body); }
            void removeBody(RigidBody* body) {
                bodies.erase(std::remove(bodies.begin(), bodies.end(), body), bodies.end());
            }

            void addCollider(CollisionPrimitive* collider) { colliders.push_back(collider); }
            void removeCollider(CollisionPrimitive* collider) {
                colliders.erase(std::remove(colliders.begin(), colliders.end(), collider), colliders.end());
            }

            // Getters
            RigidBodies& getBodies() { return bodies; }
            ForceRegistry& getForceRegistry() { return registry; }
            Colliders& getColliders() { return colliders; }
            bool* debugModeActive() { return &debugDrawEnabled; }

        protected:
            RigidBodies bodies; ///< All bodies in this world
            ForceRegistry registry; ///< All force generators in this world

            Colliders colliders; ///< Collision shapes attached to rigid bodies or immovable objects
            ContactResolver resolver; ///< Collision resolution routine for all contacts in this world
            Contact* contacts; ///< Array of contacts, for filling by the contact generators
            unsigned maxContacts; ///< Max number of contacts allowed in this world
            bool calculateIterations; ///< True if should calculate number of iterations to give to contact resolver

            bool debugDrawEnabled = false;
            DebugRenderer debugRenderer;
        private:
            Engine* engine = nullptr;
    };
}

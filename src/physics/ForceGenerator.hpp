#pragma once

#include "RigidBody.hpp"
#include <vector>

namespace Villain {

    // Can be used to add a force to one or more rigid bodies
    class ForceGenerator {
        public:
            virtual void updateForce(RigidBody* body, float deltaTime) = 0;
    };

    // Holds relationships between all the force generators and the particles they are applied to
    class ForceRegistry {
        public:
            // Register given force generator to apply to given body
            void add(RigidBody* body, ForceGenerator* generator);
            // Unregister given RigidBody/ForceGenerator pair
            void remove(RigidBody* body, ForceGenerator* generator);
            // Clears all registrations, not responsible for deleting particles and generators
            void clear();

            // Main update method to call all generators with their corresponding particles
            void updateForces(float deltaTime);


        protected:
            // 1 to 1 force generator to rigid body mapping
            struct ForceRegistration {
                RigidBody* body;
                ForceGenerator* generator;
            };

            typedef std::vector<ForceRegistration> BodyRegistry;
            BodyRegistry registry;
    };
}

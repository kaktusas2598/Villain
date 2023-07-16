#pragma once

#include "physics/ForceGenerator.hpp"

namespace Villain {

    // Applies buoyant force to rigid body
    // TODO: Properly test this
    class Buoyancy : public ForceGenerator {
        public:
            Buoyancy(const glm::vec3& cOfB, float maxDepth, float volume, float planeHeight, float density)
                : maximumDepth(maxDepth), bodyVolume(volume), waterHeight(planeHeight), liquidDensity(density),
                centreOfBuoyancy(cOfB) {}

            virtual void updateForce(RigidBody* body, float deltaTime) {
                // Calculate the submersion depth
                float depth = body->getPointInWorldSpace(centreOfBuoyancy).y;

                // Check if body is out of the water
                if (depth >= waterHeight + maximumDepth) return;
                glm::vec3 force;

                // Check if body is at the maximum depth
                if (depth <= waterHeight - maximumDepth) {
                    force.y = liquidDensity * bodyVolume;
                    body->addForceAtBodyPoint(force, centreOfBuoyancy);
                    return;
                }

                // Otherwise body is partially submerged
                force.y = liquidDensity * bodyVolume * (depth - maximumDepth - waterHeight) / 2 * maximumDepth;
                body->addForceAtBodyPoint(force, centreOfBuoyancy);
            }

        private:
            float maximumDepth; //< Maximum submersion depth of the object before it generates maximum buoyancy force
            float bodyVolume; //< Volume of the object
            float waterHeight; //< Height of the water plane above y=0, parallel to XZ plane
            float liquidDensity; //< Density of the liquid. Pure water is 1000kg/m^3
            glm::vec3 centreOfBuoyancy; //< Centre of buoyancy of the rigid body in body coordinates
    };
}

#pragma once

#include "Aero.hpp"

namespace Villain {

    // Force generator describing aerodynamic surface which can be reoriented relative to the rigid body
    // TODO: Properly test this
    class AngledAero : public Aero {
        public:
            AngledAero(const glm::mat3& aeroTensor, const glm::vec3& pos, glm::vec3* windspeed)
                : Aero(aeroTensor, pos, windspeed) {}

            // Sets the new orientation of the aerodynamic surface relative to the rigid body it is attached to, but
            // doesn't affect the point of connection of the surface to the body
            void setOrientation(const glm::quat& newOrientation) {
                orientation = newOrientation;
            }

            virtual void updateForce(RigidBody* body, float deltaTime) {
                // Calculate total velocity (wind speed and body's velocity)
                glm::vec3 velocity = body->getLinearVelocity();
                velocity += *windSpeed;

                // Calculate the velocity in body coordinates
                glm::vec3 bodyVel = glm::inverse(body->getTransform()) * glm::vec4(velocity, 1.0);

                // Calculate the force in body coordinates
                glm::vec3 bodyForce = tensor * bodyVel;
                // NOTE: No idea id usage of orienation is correct in this case
                glm::vec3 force = body->getTransform() * glm::mat4_cast(orientation) * glm::vec4(bodyForce, 1.0);

                // Finally apply the force
                body->addForceAtBodyPoint(force, position);

            }

        private:
            glm::quat orientation; //< Orientation of the aerodynamic surface relative to the rigid body it is attached to
    };
}

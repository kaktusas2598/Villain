#pragma once

#include "Aero.hpp"
#include "Logger.hpp"
#include "glm/gtc/quaternion.hpp"

namespace Villain {

    // Force generator with a control aerodynamic surface. Uses 3 inertia tensors, two for the extremes
    // and one of the 'resting' position pf the control surface. The latter tensor is inherited from base class.
    // TODO: Properly test this
    class AeroControl : public Aero {
        public:
            AeroControl(
                    const glm::mat3& base, const glm::mat3& min, const glm::mat3& max,
                    const glm::vec3& pos, glm::vec3* windspeed) :
                Aero(base, pos, windspeed), maxTensor(max), minTensor(min) {}

            // Set the control position, should be in range [-1, +1]!
            void setControl(float value) { controlSetting = value; }

            virtual void updateForce(RigidBody* body, float deltaTime) {
                Aero::updateForceFromTensor(body, deltaTime, getTensor());
            }

        private:
            glm::mat3 maxTensor; //< Aerodynamic tensor for the surface when the control is at its maximum value
            glm::mat3 minTensor; //< Aerodynamic tensor for the surface when the control is at its minimum value
            // The current position of the control for this surface. Should be in range [-1, +1], where when -1 min tensor is used,
            // at 0 base tensor is used and as +1, max tensor is ued
            float controlSetting;

            // Calculate final aerodynamic tensor for the current control setting
            glm::mat3 getTensor() {
                if (controlSetting <= -1.0f) return minTensor;
                else if(controlSetting >= 1.0f) return maxTensor;
                else if (controlSetting < 0) {
                    //return linearInterpolate(minTensor, tensor, controlSetting + 1.0f);
                    return glm::mat3_cast(glm::lerp(glm::quat_cast(minTensor), glm::quat_cast(tensor), controlSetting + 1.0f));
                    //return glm::mix(minTensor, tensor, controlSetting + 1.0f);
                } else if (controlSetting > 0) {
                    //return linearInterpolate(tensor, maxTensor, controlSetting);
                    return glm::mat3_cast(glm::lerp(glm::quat_cast(tensor), glm::quat_cast(maxTensor), controlSetting + 1.0f));
                    //return glm::mix(tensor, maxTensor, controlSetting);
                } else return tensor;
            }

            // Stack smashing!!
            glm::mat3 linearInterpolate(const glm::mat3& a, const glm::mat3& b, float prop)
                {
                    glm::mat3 result;
                    for (unsigned i = 0; i < 9; i++) {
                        result[i] = a[i] * (1-prop) + b[i] * prop;
                    }
                    return result;
                }
    };
}

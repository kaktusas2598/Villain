#pragma once

#include <glm/gtc/type_ptr.hpp>

namespace Villain {

    /*! \brief Helper util to calculate common inertia tensors.
     *
     * Calculates inertia tensors glm::mat3 for common primitive shapes. Inertia tensors
     * are convenient way to describe moment of inertia for different axis of rotations for rigid bodies.
     */
    class TensorUtils {
        public:
            /// Inertia tensor matrix for Cuboid shape
            static glm::mat3 Cuboid(const glm::vec3& size, float mass) {
                float cuboidInertiaFactor = (1.0f / 12.0f) * mass;
                float cuboidInertiaX = cuboidInertiaFactor * (size.y * size.y + size.z * size.z);
                float cuboidInertiaY = cuboidInertiaFactor * (size.x * size.x + size.z * size.z);
                float cuboidInertiaZ = cuboidInertiaFactor * (size.x * size.x + size.y * size.y);
                float cuboidInertiaTensor[9] = {
                    cuboidInertiaX, 0, 0,
                    0, cuboidInertiaY, 0,
                    0, 0, cuboidInertiaZ
                };

                return glm::make_mat3(cuboidInertiaTensor);
            }

            /// Inertia tensor matrix for Solid sphere shape
            static glm::mat3 Sphere(float radius, float mass) {
                float sphereInertia = (2.0f / 5.0f) * mass * radius * radius;
                float sphereInertiaTensor[9] = {
                    sphereInertia, 0, 0,
                    0, sphereInertia, 0,
                    0, 0, sphereInertia
                };

                return glm::make_mat3(sphereInertiaTensor);
            }
    };
}

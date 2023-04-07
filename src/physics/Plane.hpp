#ifndef __PLANE__
#define __PLANE__

#include "glm/glm.hpp"
#include "BoundingSphere.hpp"

namespace Villain {

    class Plane {
        public:
            Plane() = default;
            Plane(const glm::vec3& norm, float dist) :
                normal(norm), distance(dist) {}
            Plane(const glm::vec3& point, const glm::vec3& norm) :
                normal(glm::normalize(norm)),
                distance(glm::dot(normal, point)) {}

            Plane normalized() const;
            IntersectData intersectSphere(const BoundingSphere& sphere);

            float getSignedDistanceToPlane(const glm::vec3 point) const {
                return glm::dot(normal, point) - distance;
            }

            inline const glm::vec3& getNormal() const { return normal; }
            inline float getDistance() const { return distance; }
        private:
            // unit vector
            glm::vec3 normal = {0.f, 1.f, 0.f};
            // distance from origin to the nearest point on the plane
            float distance = 0.f;
    };

}

#endif // __PLANE__

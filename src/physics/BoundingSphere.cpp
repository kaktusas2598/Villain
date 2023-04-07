#include "BoundingSphere.hpp"

namespace Villain {

    IntersectData BoundingSphere::intersectBoundingSphere(const BoundingSphere& other) {
        float radiusDistance = radius + other.getRadius();
        float centreDistance = glm::length(other.getCentre() - centre);

        float distance = centreDistance - radiusDistance;

        return IntersectData(distance < 0, distance);
    }

    void BoundingSphere::transform(const glm::vec3& translation) {
        centre += translation;
    }
}

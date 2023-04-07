#include "BoundingSphere.hpp"

namespace Villain {

    IntersectData BoundingSphere::intersectBoundingSphere(const BoundingSphere& other) {
        float radiusDistance = radius + other.getRadius();
        glm::vec3 direction = other.getCentre() - centre;
        float centreDistance = glm::length(direction);
        direction /= centreDistance; // normalize

        float distance = centreDistance - radiusDistance;

        return IntersectData(distance < 0, direction * distance);
    }

    void BoundingSphere::transform(const glm::vec3& translation) {
        centre += translation;
    }
}

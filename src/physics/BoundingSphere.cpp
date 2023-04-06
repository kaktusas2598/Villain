#include "BoundingSphere.hpp"


namespace Villain {

    IntersectData BoundingSphere::intersectBoundingSphere(const BoundingSphere& other) {
        float radiusDistance = radius + other.getRadius();
        float centreDistance = glm::length(other.getCentre() - centre);

        if (centreDistance < radiusDistance) {
            return IntersectData(true, centreDistance - radiusDistance);
        } else {
            return IntersectData(false, centreDistance - radiusDistance);
        }
    }
}

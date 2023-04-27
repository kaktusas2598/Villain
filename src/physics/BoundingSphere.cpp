#include "BoundingSphere.hpp"

#include "rendering/DebugRenderer.hpp"

namespace Villain {

    IntersectData BoundingSphere::intersectBoundingSphere(const BoundingSphere& other) {
        float radiusDistance = radius + other.getRadius();
        glm::vec3 direction = other.getCentre() - centre;
        float centreDistance = glm::length(direction);
        direction /= centreDistance; // normalize

        float distance = centreDistance - radiusDistance;

        return IntersectData(distance < 0, direction * distance);
    }

    IntersectData BoundingSphere::intersectAABB(const BoundingAABB& other) {
        // get box closest point to sphere center by clamping
        const double x = fmax(other.getMinExtents().x, fmin(this->centre.x, other.getMaxExtents().x));
        const double y = fmax(other.getMinExtents().y, fmin(this->centre.y, other.getMaxExtents().y));
        const double z = fmax(other.getMinExtents().z, fmin(this->centre.z, other.getMaxExtents().z));

        glm::vec3 direction = {
            x - this->centre.x,
            y - this->centre.y,
            z - this->centre.z
        };

        return IntersectData(glm::length(direction) < this->radius, direction);
    }

    void BoundingSphere::transform(const glm::vec3& translation) {
        centre += translation;
    }

    void BoundingSphere::render(DebugRenderer* renderer) {
        renderer->drawSphere(centre, radius, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    }
}

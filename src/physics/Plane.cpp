#include "Plane.hpp"

#include "BoundingVolume.hpp"

namespace Villain {

    Plane Plane::normalized() const {
        float magnitude = normal.length();

        return Plane(normal/magnitude, distance/magnitude);
    }

    IntersectData Plane::intersectSphere(const BoundingSphere& sphere) {
        // NOTE: does this do same thing as getSignedDistanceToPlane(glm::vec3&)?
        // super similar
        float distanceFromSphereCentre = fabs(glm::dot(normal, sphere.getCentre()) + distance);
        float distanceFromSphere = distanceFromSphereCentre - sphere.getRadius();

        return IntersectData(distanceFromSphere < 0.f, normal * distanceFromSphere);
    }
}

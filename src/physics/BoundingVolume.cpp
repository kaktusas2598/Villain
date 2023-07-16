#include "BoundingVolume.hpp"

#include "Frustum.hpp"
#include "Logger.hpp"
#include "Transform.hpp"

namespace Villain {

    bool BoundingVolume::isOnFrustum(const Frustum& camFrustum) const {
        return (isOnOrForwardPlane(camFrustum.leftFace) &&
                isOnOrForwardPlane(camFrustum.rightFace) &&
                isOnOrForwardPlane(camFrustum.topFace) &&
                isOnOrForwardPlane(camFrustum.bottomFace) &&
                isOnOrForwardPlane(camFrustum.nearFace) &&
                isOnOrForwardPlane(camFrustum.farFace));
    }

    IntersectData BoundingVolume::intersect(const BoundingVolume& other) const {

        if (type == BoundingVolumeType::SPHERE && other.getType() == BoundingVolumeType::SPHERE) {
            // Doesn't seem like a good way because of those casts
            BoundingSphere* self = (BoundingSphere*)this;
            return self->intersectBoundingSphere((BoundingSphere&)other);
        } else if (type == BoundingVolumeType::AABB && other.getType() == BoundingVolumeType::AABB) {
            // Doesn't seem like a good way because of those casts
            BoundingAABB* self = (BoundingAABB*)this;
            return self->intersectAABB((BoundingAABB&)other);
        } else if (type == BoundingVolumeType::SPHERE && other.getType() == BoundingVolumeType::AABB) {
            BoundingSphere* self = (BoundingSphere*)this;
            return self->intersectAABB((BoundingAABB&)other);
        } else {
            VILLAIN_ERROR("Collision type not supported");
            return IntersectData(false, glm::vec3(-1.f));
        }
    }

    bool BoundingSphere::isOnFrustum(const Frustum& camFrustum, Transform& transform) const {
        const glm::vec3 globalScale = transform.getGlobalScale();

        // Get our global center using global model matrix of the transform
        //const glm::vec3 globalCenter{ transform.getModelMatrix() * glm::vec4(center, 1.f) };
        const glm::vec3 globalCenter{ transform.getTransformMatrix() * glm::vec4(Center, 1.f) };

        //To wrap correctly our shape, we need the maximum scale scalar.
        const float maxScale = std::max(std::max(globalScale.x, globalScale.y), globalScale.z);

        //Max scale is assuming for the diameter. So, we need the half to apply it to our radius
        BoundingSphere globalSphere(globalCenter, Radius * (maxScale * 0.5f));

        //Check Firstly the result that have the most chance to failure to avoid to call all functions.
        return (globalSphere.isOnOrForwardPlane(camFrustum.leftFace) &&
                globalSphere.isOnOrForwardPlane(camFrustum.rightFace) &&
                globalSphere.isOnOrForwardPlane(camFrustum.farFace) &&
                globalSphere.isOnOrForwardPlane(camFrustum.nearFace) &&
                globalSphere.isOnOrForwardPlane(camFrustum.topFace) &&
                globalSphere.isOnOrForwardPlane(camFrustum.bottomFace));
    }

    IntersectData BoundingSphere::intersectBoundingSphere(const BoundingSphere& other) {
        float radiusDistance = Radius + other.getRadius();
        glm::vec3 direction = other.getCentre() - Center;
        float centreDistance = glm::length(direction);
        direction /= centreDistance; // normalize

        float distance = centreDistance - radiusDistance;

        return IntersectData(distance < 0, direction * distance);
    }

    IntersectData BoundingSphere::intersectAABB(const BoundingAABB& other) {
        // get box closest point to sphere center by clamping
        const double x = fmax(other.getMinExtents().x, fmin(this->Center.x, other.getMaxExtents().x));
        const double y = fmax(other.getMinExtents().y, fmin(this->Center.y, other.getMaxExtents().y));
        const double z = fmax(other.getMinExtents().z, fmin(this->Center.z, other.getMaxExtents().z));

        glm::vec3 direction = {
            x - this->Center.x,
            y - this->Center.y,
            z - this->Center.z
        };

        return IntersectData(glm::length(direction) < this->Radius, direction);
    }

    void BoundingSphere::transform(const glm::vec3& translation) {
        Center += translation;
    }

    void BoundingSphere::render(DebugRenderer* renderer) {
        renderer->drawSphere(Center, Radius, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    }

    bool BoundingAABB::isOnFrustum(const Frustum& camFrustum, Transform& transform) const {
        const glm::vec3 globalCenter{ transform.getTransformMatrix() * glm::vec4(Center, 1.f) };

        // Scaled orientation
        const glm::vec3 right = transform.getRight() * Extents.x;
        const glm::vec3 up = transform.getUp() * Extents.y;
        const glm::vec3 forward = transform.getForward() * Extents.z;

        const float newIi = std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, right)) +
            std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, up)) +
            std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, forward));

        const float newIj = std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, right)) +
            std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, up)) +
            std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, forward));

        const float newIk = std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, right)) +
            std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, up)) +
            std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, forward));

        //We not need to divise scale because it's based on the half extention of the AABB
        const BoundingAABB globalAABB(globalCenter, newIi, newIj, newIk);

        return (globalAABB.isOnOrForwardPlane(camFrustum.leftFace) &&
                globalAABB.isOnOrForwardPlane(camFrustum.rightFace) &&
                globalAABB.isOnOrForwardPlane(camFrustum.topFace) &&
                globalAABB.isOnOrForwardPlane(camFrustum.bottomFace) &&
                globalAABB.isOnOrForwardPlane(camFrustum.nearFace) &&
                globalAABB.isOnOrForwardPlane(camFrustum.farFace));
    };

    IntersectData BoundingAABB::intersectAABB(const BoundingAABB& other) {
        glm::vec3 distances1 = other.getMinExtents() - getMaxExtents();
        glm::vec3 distances2 = getMinExtents() - other.getMaxExtents();
        //glm::vec3 distances = distances1.max(distances2);
        // Not sure if this is correct
        glm::vec3 distances = glm::max(distances1, distances2);
        //glm::vec3 distances = glm::greaterThan(distances1, distances2);

        // My Ridiculous and super confusing way to get max component of vec3!
        // Cause I couldn't find function for that in glm :/
        float maxDistance = (distances.x > distances.y) ?
            ((distances.x > distances.z) ? distances.x : distances.z) :
            ((distances.y > distances.z) ? distances.y: distances.z);

        return IntersectData(maxDistance < 0, distances);
    }

    void BoundingAABB::transform(const glm::vec3& translation) {
        Center += translation;
    }

    void BoundingAABB::render(DebugRenderer* renderer) {
        renderer->drawBox3D(Center, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), Extents);
    }
}

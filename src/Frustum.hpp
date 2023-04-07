#ifndef __FRUSTUM__
#define __FRUSTUM__

#include "physics/Plane.hpp"
#include "Transform.hpp"
#include "glm/glm.hpp"

namespace Villain {

    struct Frustum {
        Plane topFace;
        Plane bottomFace;

        Plane leftFace;
        Plane rightFace;

        Plane nearFace;
        Plane farFace;
    };

    // Provides a simple shape to wrap mesh in which can be used in collision detection or frustum culling
    // TODO: this can probably go to new class, which can also be used for collisions?
    struct BoundingVolume {
        virtual bool isOnFrustum(const Frustum& camFrustum, Transform& transform) const = 0;

        virtual bool isOnOrForwardPlane(const Plane& plane) const = 0;

        bool isOnFrustum(const Frustum& camFrustum) const {
            return (isOnOrForwardPlane(camFrustum.leftFace) &&
                    isOnOrForwardPlane(camFrustum.rightFace) &&
                    isOnOrForwardPlane(camFrustum.topFace) &&
                    isOnOrForwardPlane(camFrustum.bottomFace) &&
                    isOnOrForwardPlane(camFrustum.nearFace) &&
                    isOnOrForwardPlane(camFrustum.farFace));
        }
    };

    struct Sphere : public BoundingVolume {
        glm::vec3 Center{0.f, 0.f , 0.f};
        float Radius = 0.f;

        Sphere(const glm::vec3& center, float radius)
		: BoundingVolume(), Center(center), Radius(radius) {}

        virtual bool isOnOrForwardPlane(const Plane& plane) const final {
            return plane.getSignedDistanceToPlane(Center) > -Radius;
        }

        virtual bool isOnFrustum(const Frustum& camFrustum, Transform& transform) const final {
            const glm::vec3 globalScale = transform.getGlobalScale();

            // Get our global center using global model matrix of the transform
            //const glm::vec3 globalCenter{ transform.getModelMatrix() * glm::vec4(center, 1.f) };
            const glm::vec3 globalCenter{ transform.getTransformMatrix() * glm::vec4(Center, 1.f) };

            //To wrap correctly our shape, we need the maximum scale scalar.
            const float maxScale = std::max(std::max(globalScale.x, globalScale.y), globalScale.z);

            //Max scale is assuming for the diameter. So, we need the half to apply it to our radius
            Sphere globalSphere(globalCenter, Radius * (maxScale * 0.5f));

            //Check Firstly the result that have the most chance to failure to avoid to call all functions.
            return (globalSphere.isOnOrForwardPlane(camFrustum.leftFace) &&
                globalSphere.isOnOrForwardPlane(camFrustum.rightFace) &&
                globalSphere.isOnOrForwardPlane(camFrustum.farFace) &&
                globalSphere.isOnOrForwardPlane(camFrustum.nearFace) &&
                globalSphere.isOnOrForwardPlane(camFrustum.topFace) &&
                globalSphere.isOnOrForwardPlane(camFrustum.bottomFace));
        }
    };

    struct AABB : public BoundingVolume {
        glm::vec3 Center{0.f, 0.f, 0.f};
        glm::vec3 Extents{0.f, 0.f, 0.f};

        AABB(const glm::vec3& min, const glm::vec3& max) :
            BoundingVolume(), Center((max + min) * 0.5f), Extents(max.x - Center.x, max.y - Center.y, max.z - Center.z) {}

        AABB(const glm::vec3& center, float iI, float iJ, float iK) :
            BoundingVolume(), Center(center), Extents(iI, iJ, iK) {}

        virtual bool isOnOrForwardPlane(const Plane& plane) const {
            // Compute the projection interval radius of b onto L(t) = b.c + t * p.n
            const float r = Extents.x * std::abs(plane.getNormal().x) +
                    Extents.y * std::abs(plane.getNormal().y) + Extents.z * std::abs(plane.getNormal().z);

            return -r <= plane.getSignedDistanceToPlane(Center);
        }

        virtual bool isOnFrustum(const Frustum& camFrustum, Transform& transform) const final {
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
            const AABB globalAABB(globalCenter, newIi, newIj, newIk);

            return (globalAABB.isOnOrForwardPlane(camFrustum.leftFace) &&
                globalAABB.isOnOrForwardPlane(camFrustum.rightFace) &&
                globalAABB.isOnOrForwardPlane(camFrustum.topFace) &&
                globalAABB.isOnOrForwardPlane(camFrustum.bottomFace) &&
                globalAABB.isOnOrForwardPlane(camFrustum.nearFace) &&
                globalAABB.isOnOrForwardPlane(camFrustum.farFace));
        };
    };
}

#endif // __FRUSTUM__

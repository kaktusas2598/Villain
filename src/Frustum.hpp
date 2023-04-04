#ifndef __FRUSTUM__
#define __FRUSTUM__

//#include "Camera3D.hpp"
#include "Transform.hpp"
#include "glm/glm.hpp"

namespace Villain {

    struct Plane {
        // unit vector
        glm::vec3 Normal = {0.f, 1.f, 0.f};

        // distance from origin to the nearest point on the plane
        float Distance = 0.f;

        Plane() = default;
        Plane(const glm::vec3& point, const glm::vec3& normal) :
            Normal(glm::normalize(normal)),
            Distance(glm::dot(Normal, point)) {}

        float getSignedDistanceToPlane(const glm::vec3 point) const {
            return glm::dot(Normal, point) - Distance;
        }
    };

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

    // TODO: implement AABB bounding volume
}

#endif // __FRUSTUM__

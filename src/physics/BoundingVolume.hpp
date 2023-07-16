#pragma once

#include "IntersectData.hpp"
#include "Plane.hpp"
#include "rendering/DebugRenderer.hpp"

namespace Villain {

    class Plane;
    class Transform;
    struct Frustum;

    enum class BoundingVolumeType {
        SPHERE, AABB,
        TYPE_SIZE
    };

    // Provides a simple shape to wrap mesh in which can be used in collision detection or frustum culling
    struct BoundingVolume {
        BoundingVolume(BoundingVolumeType volumeType): type(volumeType) {}

        virtual bool isOnFrustum(const Frustum& camFrustum, Transform& transform) const = 0;
        virtual bool isOnOrForwardPlane(const Plane& plane) const = 0;
        bool isOnFrustum(const Frustum& camFrustum) const;

        IntersectData intersect(const BoundingVolume& other) const;
        virtual void transform(const glm::vec3& translation) {}
        virtual const glm::vec3& getCentre() const = 0;
        virtual void render(DebugRenderer* renderer) = 0;

        inline BoundingVolumeType getType() const { return type; }

        private:
        BoundingVolumeType type;
    };

    struct BoundingAABB;
    struct BoundingSphere : public BoundingVolume {
        glm::vec3 Center{0.f, 0.f , 0.f};
        float Radius = 0.f;

        BoundingSphere(const glm::vec3& center, float radius)
            : BoundingVolume(BoundingVolumeType::SPHERE), Center(center), Radius(radius) {}

        virtual bool isOnOrForwardPlane(const Plane& plane) const final {
            return plane.getSignedDistanceToPlane(Center) > -Radius;
        }

        virtual bool isOnFrustum(const Frustum& camFrustum, Transform& transform) const final;

        IntersectData intersectBoundingSphere(const BoundingSphere& other);
        IntersectData intersectAABB(const BoundingAABB& other);
        virtual void transform(const glm::vec3& translation);

        virtual void render(DebugRenderer* renderer);

        virtual const glm::vec3& getCentre() const { return Center; }
        inline float getRadius() const { return Radius; }
    };

    struct BoundingAABB : public BoundingVolume {
        glm::vec3 Center{0.f, 0.f, 0.f};
        glm::vec3 Extents{0.f, 0.f, 0.f};

        BoundingAABB(const glm::vec3& min, const glm::vec3& max) :
            BoundingVolume(BoundingVolumeType::AABB), Center((max + min) * 0.5f), Extents(max.x - Center.x, max.y - Center.y, max.z - Center.z) {}

        BoundingAABB(const glm::vec3& center, float iI, float iJ, float iK) :
            BoundingVolume(BoundingVolumeType::AABB), Center(center), Extents(iI, iJ, iK) {}

        virtual bool isOnOrForwardPlane(const Plane& plane) const {
            // Compute the projection interval radius of b onto L(t) = b.c + t * p.n
            const float r = Extents.x * std::abs(plane.getNormal().x) +
                Extents.y * std::abs(plane.getNormal().y) + Extents.z * std::abs(plane.getNormal().z);

            return -r <= plane.getSignedDistanceToPlane(Center);
        }

        virtual bool isOnFrustum(const Frustum& camFrustum, Transform& transform) const final;

        IntersectData intersectAABB(const BoundingAABB& other);
        virtual void transform(const glm::vec3& translation);
        virtual const glm::vec3& getCentre() const {
            return Center;
            //glm::vec3 minExtents = getMinExtents();
            //glm::vec3 maxExtents = getMaxExtents();
            //glm::vec3 halfSize = (maxExtents - minExtents) / 2.f;
            //// Basically tell compiler that even though this is const method, we know that we're doing
            //((BoundingAABB&)(*this)).centre = minExtents + halfSize;;
            //return centre;
        }

        virtual void render(DebugRenderer* renderer);

        glm::vec3 getMinExtents() const { return Center - Extents; }
        glm::vec3 getMaxExtents() const { return Center + Extents; }
    };
}

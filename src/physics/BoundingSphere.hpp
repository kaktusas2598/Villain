#ifndef __BOUNDING_SPHERE__
#define __BOUNDING_SPHERE__

#include "Collider.hpp"
#include "BoundingAABB.hpp"

namespace Villain {

    // Simplest type of bounding volume
    // NOTE: Should be refactored because it duplicates some functionality from Frustum.hpp
    class BoundingSphere : public Collider {
        public:
            BoundingSphere(const glm::vec3& c, float r)
                : Collider(ColliderType::SPHERE), centre(c), radius(r) {}

            IntersectData intersectBoundingSphere(const BoundingSphere& other);
            IntersectData intersectAABB(const BoundingAABB& other);
            virtual void transform(const glm::vec3& translation);

            virtual void render(DebugRenderer* renderer);

            virtual const glm::vec3& getCentre() const { return centre; }
            inline float getRadius() const { return radius; }
        private:
            // Temporary removed const here to be able to assign BoundingSphere in physics object, will have to change
            glm::vec3 centre;
            float radius;
    };
}

#endif // __BOUNDING_SPHERE__

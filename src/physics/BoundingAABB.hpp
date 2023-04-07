#ifndef __BOUNDING_AABB__
#define __BOUNDING_AABB__

#include "Collider.hpp"

namespace Villain {

    class BoundingAABB : public Collider {
        public:
            BoundingAABB(const glm::vec3& min, const glm::vec3& max) :
                Collider(ColliderType::AABB), minExtents(min), maxExtents(max) {}

            IntersectData intersectAABB(const BoundingAABB& other);
            // TODO:
            virtual void transform(const glm::vec3& translation) {}
            // TODO:
            virtual const glm::vec3& getCentre() const { return maxExtents - minExtents; }

            const glm::vec3& getMinExtents() const { return minExtents; }
            const glm::vec3& getMaxExtents() const { return maxExtents; }
        private:
            glm::vec3 minExtents;
            glm::vec3 maxExtents;
    };
}

#endif // __BOUNDING_AABB__

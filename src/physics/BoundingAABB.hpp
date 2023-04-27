#ifndef __BOUNDING_AABB__
#define __BOUNDING_AABB__

#include "Collider.hpp"

namespace Villain {

    class BoundingAABB : public Collider {
        public:
            BoundingAABB(const glm::vec3& min, const glm::vec3& max) :
                Collider(ColliderType::AABB), minExtents(min), maxExtents(max) {}

            IntersectData intersectAABB(const BoundingAABB& other);
            virtual void transform(const glm::vec3& translation);
            virtual const glm::vec3& getCentre() const {
                glm::vec3 halfSize = (maxExtents - minExtents) / 2.f;
                // Basically tell compiler that even though this is const method, we know that we're doing
                ((BoundingAABB&)(*this)).centre = minExtents + halfSize;;
                return centre;
            }

            virtual void render(DebugRenderer* renderer);

            const glm::vec3& getMinExtents() const { return minExtents; }
            const glm::vec3& getMaxExtents() const { return maxExtents; }
        private:
            void recalculateCentre() {
                glm::vec3 halfSize = (maxExtents - minExtents) / 2.f;
                centre = minExtents + halfSize;
            }
            glm::vec3 minExtents;
            glm::vec3 maxExtents;

            // Recalculated
            glm::vec3 centre;
    };
}

#endif // __BOUNDING_AABB__

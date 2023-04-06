#ifndef __BOUNDING_AABB__
#define __BOUNDING_AABB__

#include "glm/glm.hpp"
#include "IntersectData.hpp"

namespace Villain {

    class BoundingAABB {
        public:
            BoundingAABB(const glm::vec3& min, const glm::vec3& max) :
                minExtents(min), maxExtents(max) {}

            IntersectData intersectAABB(const BoundingAABB& other);

            const glm::vec3& getMinExtents() const { return minExtents; }
            const glm::vec3& getMaxExtents() const { return maxExtents; }
        private:
            const glm::vec3 minExtents;
            const glm::vec3 maxExtents;
    };
}

#endif // __BOUNDING_AABB__

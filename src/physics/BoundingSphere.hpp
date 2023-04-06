#ifndef __BOUNDING_SPHERE__
#define __BOUNDING_SPHERE__

#include "glm/glm.hpp"
#include "IntersectData.hpp"

namespace Villain {

    // Simplest type of bounding volume
    // NOTE: Should be refactored because it duplicates some functionality from Frustum.hpp
    class BoundingSphere {
        public:
            BoundingSphere(const glm::vec3& c, float r)
                : centre(c), radius(r) {}

            IntersectData intersectBoundingSphere(const BoundingSphere& other);

            inline const glm::vec3& getCentre() const { return centre; }
            inline float getRadius() const { return radius; }
        private:
            const glm::vec3 centre;
            const float radius;
    };
}

#endif // __BOUNDING_SPHERE__

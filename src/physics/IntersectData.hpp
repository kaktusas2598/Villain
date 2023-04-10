#ifndef __INTERSECT_DATA__
#define __INTERSECT_DATA__

#include "glm/glm.hpp"

namespace Villain {

    class IntersectData {
        public:
            IntersectData(const bool intersect, const glm::vec3& dir) :
                intersected(intersect), direction(dir) {}

            inline bool isIntersecting() const { return intersected; }
            inline const glm::vec3& getDirection() const { return direction; }
            inline float getDistance() const { return direction.length(); }
        private:
            const bool intersected;

            // Stores direction of collision and length of this vector is distance
            const glm::vec3 direction;
    };
}

#endif // __INTERSECT_DATA__

#pragma once

#include "glm/glm.hpp"

namespace Villain {

    // Collision Points, normal
    class IntersectData {
        public:
            IntersectData(const bool intersect, const glm::vec3& dir) :
                intersected(intersect), direction(dir) {}

            inline bool isIntersecting() const { return intersected; }
            inline const glm::vec3& getDirection() const { return direction; }
            inline float getDistance() const { return direction.length(); }
        private:
            const bool intersected;
            // TODO: utilise these instead of direction, normal will help
            // in solving impulses
            glm::vec3 A; // furthest point of A into B
            glm::vec3 B; // furthest point of B into A
            glm::vec3 normal; // normal(B - A)
            glm::vec3 depth; // length(B - A)

            // Stores direction of collision and length of this vector is distance
            const glm::vec3 direction;
    };
}

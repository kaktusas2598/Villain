#include "BoundingAABB.hpp"

#include "glm/gtx/component_wise.hpp"

namespace Villain {

    IntersectData BoundingAABB::intersectAABB(const BoundingAABB& other) {
        glm::vec3 distances1 = other.getMinExtents() - maxExtents;
        glm::vec3 distances2 = minExtents - other.getMaxExtents();
        //glm::vec3 distances = distances1.max(distances2);
        // Not sure if this is correct
        glm::vec3 distances = glm::max(distances1, distances2);
        //glm::vec3 distances = glm::greaterThan(distances1, distances2);

        // My Ridiculous and super confusing way to get max component of vec3!
        // Cause I couldn't find function for that in glm :/
        float maxDistance = (distances.x > distances.y) ?
            ((distances.x > distances.z) ? distances.x : distances.z) :
            ((distances.y > distances.z) ? distances.y: distances.z);

        return IntersectData(maxDistance < 0, maxDistance);
    }
}

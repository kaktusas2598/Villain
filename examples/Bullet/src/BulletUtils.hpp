#pragma once

#include "glm/glm.hpp"
#include "LinearMath/btVector3.h"

class BulletUtils {
    public:
        static glm::vec3 toGlm(const btVector3& btVec) {
            return glm::vec3(btVec.getX(), btVec.getY(), btVec.getZ());
        }

        static btVector3 toBt(const glm::vec3& glmVec) {
            return btVector3(glmVec.z, glmVec.y, glmVec.z);
        }
};

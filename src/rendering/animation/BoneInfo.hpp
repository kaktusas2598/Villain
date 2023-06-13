#ifndef __BONE_INFO__
#define __BONE_INFO__

#include "glm/glm.hpp"

namespace Villain {

    struct BoneInfo {
        // index in finalBoneMatrices
        int id;
        // Offset matrix transforms vertex from model/local space to bone space
        glm::mat4 offset;
    };
};

#endif // __BONE_INFO__

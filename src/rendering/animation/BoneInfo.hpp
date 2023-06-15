#ifndef __BONE_INFO__
#define __BONE_INFO__

#include "glm/glm.hpp"

namespace Villain {

    // Maximum number of bones per model
    constexpr int MAX_BONES = 100;

    // Maximum number of bones influencing single vertex
    constexpr int MAX_BONE_INFLUENCE = 4;

    struct BoneInfo {
        // index in finalBoneMatrices
        int id;
        // Offset matrix transforms vertex from model/local space to bone space
        glm::mat4 offset;
    };
};

#endif // __BONE_INFO__

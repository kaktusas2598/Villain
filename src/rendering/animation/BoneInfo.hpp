#ifndef __BONE_INFO__
#define __BONE_INFO__

#include "glm/glm.hpp"

namespace Villain {

    struct BoneInfo {
        // index in finalBoneMatrices
        int id;
        // Offset matrix transforms vertex from model/local space to bone space A.K.A. Inverse Bind Pose Matrix!
        // It includes all transformations in hierarchy for any bone so no need to worry about that
        glm::mat4 offset;
        // TODO: also store finalTransformation here so we don't need to recalculate it every time in animator
        // this can be calculated in Model class on load
        glm::mat4 finalTransformation{1.0f};
    };
};

#endif // __BONE_INFO__

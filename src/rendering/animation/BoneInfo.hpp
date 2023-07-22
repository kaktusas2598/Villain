#pragma once

#include "glm/glm.hpp"

namespace Villain {

    struct BoneInfo {
        // index in finalBoneMatrices
        int id;
        // Offset matrix transforms vertex from model/local space to bone space A.K.A. Inverse Bind Pose Matrix!
        // It includes all transformations in hierarchy for any bone so no need to worry about that
        glm::mat4 offset;
    };
};

#ifndef __ANIMATOR__
#define __ANIMATOR__

#include "Animation.hpp"

namespace Villain {

    // Reads bone hierarchy, interpolates between all of them recursively and calculates final transforms
    class Animator {
        public:
            Animator(Animation* animation);

            void updateAnimation(float dt);
            void playAnimation(Animation* animation);
            void calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);

            std::vector<glm::mat4> getFinalBoneMatrics() { return finalBoneMatrices; }
        private:
            std::vector<glm::mat4> finalBoneMatrices;
            Animation* currentAnimation;
            float currentTime;
            float deltaTime;
    };
}

#endif // __ANIMATOR__

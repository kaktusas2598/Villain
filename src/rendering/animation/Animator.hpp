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
            void stopAnimation() { currentAnimation = nullptr; }
            void calculateBoneTransform(const AssimpNodeData* node, const glm::mat4& parentTransform);

            std::vector<glm::mat4>& getFinalBoneMatrices() { return finalBoneMatrices; }
        private:
            std::vector<glm::mat4> finalBoneMatrices;
            Animation* currentAnimation = nullptr;
            float currentTime;
    };
}

#endif // __ANIMATOR__

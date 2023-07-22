#pragma once

#include "Animation.hpp"

namespace Villain {

    // Reads bone hierarchy, interpolates between all of them recursively and calculates final transforms
    class Animator {
        public:
            Animator(Animation* animation);

            void updateAnimation(float dt);
            void playAnimation(Animation* animation);
            void stopAnimation() { currentAnimation = nullptr; }
            void pauseAnimation() { isPaused = true; }
            void resumeAnimation() { isPaused = false; }
            void toggleAnimation() { isPaused = !isPaused; }
            bool* getBindPose() { return &bindPoseOnly; }
            void calculateBoneTransform(const AssimpNodeData* node, const glm::mat4& parentTransform);

            float* getCurrentTime() { return &currentTime; }
            Animation* getCurrentAnimation() { return currentAnimation; }

            std::vector<glm::mat4>& getFinalBoneMatrices() { return finalBoneMatrices; }
        private:
            std::vector<glm::mat4> finalBoneMatrices;
            Animation* currentAnimation = nullptr;
            float currentTime;
            bool isPaused = false;
            bool bindPoseOnly = false;
    };
}

#include "Animator.hpp"

namespace Villain {

    Animator::Animator(Animation* animation) {
        currentTime = 0.0f;
        currentAnimation = animation;
        //printf("Animation duration: %f Ticks per second: %f\n", animation->getDuration(), animation->getTicksPerSecond());
        //NOTE: animation time in seconds: duraction/ticksPerSeconds

        finalBoneMatrices.reserve(MAX_BONES);
        for (int i = 0; i < MAX_BONES; i++) {
            finalBoneMatrices.push_back(glm::mat4(1.0f));
        }
    }

    void Animator::updateAnimation(float dt) {
        if (currentAnimation) {
            currentTime += currentAnimation->getTicksPerSecond() * dt;
            currentTime = fmod(currentTime, currentAnimation->getDuration());
            calculateBoneTransform(&currentAnimation->getRootNode(), glm::mat4(1.0f));
        }
    }

    void Animator::playAnimation(Animation* animation) {
        currentAnimation = animation;
        currentTime = 0.0f;
    }

    void Animator::calculateBoneTransform(const AssimpNodeData* node, const glm::mat4& parentTransform) {
        std::string nodeName = node->Name;
        glm::mat4 nodeTransform = node->Transformation;

        Bone* bone = currentAnimation->findBone(nodeName);
        if (bone) {
            bone->update(currentTime);
            nodeTransform = bone->getLocalTransform();
        }

        // NOTE: do we need to also apply global inverse transform from assimp??
        glm::mat4 globalTransform = parentTransform * nodeTransform;
        auto boneInfoMap = currentAnimation->getBoneInfoMap();
        if (boneInfoMap.find(nodeName) != boneInfoMap.end()) {
            int index = boneInfoMap[nodeName].id;
            glm::mat4 offset = boneInfoMap[nodeName].offset;
            finalBoneMatrices[index] = currentAnimation->getGlobalInverseTransform() * globalTransform * offset;
        }

        for (int i = 0; i < node->ChildrenCount; i++)
            calculateBoneTransform(&node->Children[i], globalTransform);
    }
}

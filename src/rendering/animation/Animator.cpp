#include "Animator.hpp"

namespace Villain {

    Animator::Animator(Animation* animation) {
        currentTime = 0.0f;
        currentAnimation = animation;

        finalBoneMatrices.reserve(100);
        for (int i = 0; i < 100; i++) {
            finalBoneMatrices.push_back(glm::mat4(1.0f));
        }
    }

    void Animator::updateAnimation(float dt) {
        deltaTime = dt;
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

    void Animator::calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform) {
        std::string nodeName = node->Name;
        glm::mat4 nodeTransform = node->Transformation;

        Bone* bone = currentAnimation->findBone(nodeName);
        if (bone) {
            bone->update(currentTime);
            nodeTransform = bone->getLocalTransform();
        }

        glm::mat4 globalTransform = parentTransform * nodeTransform;
        auto boneInfoMap = currentAnimation->getBoneInfoMap();
        if (boneInfoMap.find(nodeName) != boneInfoMap.end()) {
            int index = boneInfoMap[nodeName].id;
            glm::mat4 offset = boneInfoMap[nodeName].offset;
            finalBoneMatrices[index] = globalTransform * offset;
        }

        for (int i = 0; i < node->ChildrenCount; i++)
            calculateBoneTransform(&node->Children[i], globalTransform);
    }
}

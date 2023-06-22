#include "Animator.hpp"
#include "Engine.hpp"

namespace Villain {

    Animator::Animator(Animation* animation) {
        currentTime = 0.0f;
        currentAnimation = animation;

        finalBoneMatrices.reserve(MAX_BONES);
        for (int i = 0; i < MAX_BONES; i++) {
            finalBoneMatrices.push_back(glm::mat4(1.0f));
        }
    }

    void Animator::updateAnimation(float dt) {
        if (currentAnimation) {
            // Using speed factor seems a bit too slow, but only using ticks per second makes animation too fast
            //currentTime += (Engine::getFps()/currentAnimation->getTicksPerSecond()) * dt;
            if (!isPaused) {
                // TODO: instead of diving by 2, add speed member variable to constrol animation speed
                currentTime += currentAnimation->getTicksPerSecond()/2 * dt;
            }
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
        if (bone && !bindPoseOnly) {
            bone->update(currentTime);
            nodeTransform = bone->getLocalTransform();
        }

        // TODO: instead of calculating these every single frame, need to calculate Bone transforms once then loading model
        // and only calculate animation data here
        glm::mat4 globalTransform = parentTransform * nodeTransform;
        auto boneInfoMap = currentAnimation->getBoneInfoMap();
        if (boneInfoMap.find(nodeName) != boneInfoMap.end()) {
            int index = boneInfoMap[nodeName].id;
            // Get inverse bind pose matrix for bone from aiBone struct in aiMesh
            glm::mat4 offset = boneInfoMap[nodeName].offset;
            // Calculate final bone matrix transformation
            finalBoneMatrices[index] = currentAnimation->getGlobalInverseTransform() * globalTransform * offset;
        }

        for (int i = 0; i < node->ChildrenCount; i++)
            calculateBoneTransform(&node->Children[i], globalTransform);
    }
}

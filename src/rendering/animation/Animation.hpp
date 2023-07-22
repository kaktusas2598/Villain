#pragma once

#include <assimp/Importer.hpp>
#include "Bone.hpp"
#include "BoneInfo.hpp"
#include "rendering/AssimpUtils.hpp"
#include "rendering/Model.hpp"

namespace Villain {

    // Isolates internal Assimp's structures for Animation class
    struct AssimpNodeData {
        glm::mat4 Transformation;
        std::string Name;
        int ChildrenCount;
        std::vector<AssimpNodeData> Children;
    };

    // Reads data from aiAnimation struct and creates hierarchical graph of bone nodes
    class Animation {
        public:
            Animation() = default;
            Animation(Model* model, const aiAnimation* animation, const aiNode* root, float speed = 0);
            ~Animation() {}

            Bone* findBone(const std::string& name);
            void setSpeed(float speed) { ticksPerSecond = speed; }

            inline const std::string& getName() const { return name; }
            inline float* getSpeed() { return &ticksPerSecond; }
            inline float getTicksPerSecond() { return ticksPerSecond; }
            inline float getDuration() { return duration; }
            inline const AssimpNodeData& getRootNode() { return rootNode; }
            inline const std::map<std::string, BoneInfo>& getBoneInfoMap() { return boneInfoMap; }
            inline glm::mat4 getGlobalInverseTransform() { return AssimpUtils::aiMatrixToGLM(globalInverseTransform); }

        private:
            void readMissingBones(const aiAnimation* animation, Model& model);
            void readHierarchyData(AssimpNodeData& dest, const aiNode* src);

            std::string name;
            float duration;
            float ticksPerSecond;
            std::vector<Bone> bones;
            AssimpNodeData rootNode;
            std::map<std::string, BoneInfo> boneInfoMap;
            // Brings back model from world to local space, applied last
            aiMatrix4x4 globalInverseTransform;
    };
}

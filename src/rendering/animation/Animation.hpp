#ifndef __ANIMATION__
#define __ANIMATION__


#include "Bone.hpp"
#include "BoneInfo.hpp"
#include "rendering/Model.hpp"

namespace Villain {

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
            Animation(const std::string& path, Model* model);
            ~Animation() {}

            Bone* findBone(const std::string& name);

            inline float getTicksPerSecond() { return ticksPerSecond; }
            inline float getDuration() { return duration; }
            inline const AssimpNodeData& getRootNode() { return rootNode; }
            inline const std::map<std::string, BoneInfo>& getBoneInfoMap() { return boneInfoMap; }

        private:
            void readMissingBones(const aiAnimation* animation, Model& model);
            void readHierarchyData(AssimpNodeData& dest, const aiNode* src);

            float duration;
            int ticksPerSecond;
            std::vector<Bone> bones;
            AssimpNodeData rootNode;
            std::map<std::string, BoneInfo> boneInfoMap;
    };
}

#endif // __ANIMATION__

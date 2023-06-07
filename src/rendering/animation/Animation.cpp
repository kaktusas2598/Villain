#include "Animation.hpp"

#include <algorithm>
#include <assimp/Importer.hpp>

#include "rendering/AssimpUtils.hpp"

namespace Villain {

    Animation::Animation(const std::string& path, Model* model) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate);
        assert(scene && scene->mRootNode);
        auto animation = scene->mAnimations[0];
        duration = animation->mDuration;
        ticksPerSecond = animation->mTicksPerSecond;
        readHierarchyData(rootNode, scene->mRootNode);
        readMissingBones(animation, *model);
    }

    Bone* Animation::findBone(const std::string& name) {
        auto iter = std::find_if(bones.begin(), bones.end(),
             [&](const Bone& bone)
             {
                 return bone.getName() == name;
             }
        );
        if (iter == bones.end()) return nullptr;
        else return &(*iter);
    }

    void Animation::readMissingBones(const aiAnimation* animation, Model& model) {
        int size = animation->mNumChannels;

        auto& boneMap = model.getBoneInfoMap();
        int& boneCount = model.getBoneCount();

        // channels: bones engaged in animation
        for (int i = 0; i < size; i++) {
            auto channel = animation->mChannels[i];
            std::string boneName = channel->mNodeName.data;

            if (boneMap.find(boneName) == boneMap.end()) {
                boneMap[boneName].id = boneCount;
                boneCount++;
            }
            bones.push_back(Bone(channel->mNodeName.data, boneMap[channel->mNodeName.data].id, channel));
        }

        boneInfoMap = boneMap;
    }

    void Animation::readHierarchyData(AssimpNodeData& dest, const aiNode* src) {
        assert(src);

        dest.Name = src->mName.data;
        dest.Transformation = AssimpUtils::aiMatrixToGLM(src->mTransformation);
        dest.ChildrenCount = src->mNumChildren;

        for (int i = 0; i < src->mNumChildren; i++) {
            AssimpNodeData data;
            readHierarchyData(data, src->mChildren[i]);
            dest.Children.push_back(data);
        }
    }
}

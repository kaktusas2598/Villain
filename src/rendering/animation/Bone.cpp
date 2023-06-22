#include "Bone.hpp"

#include "rendering/AssimpUtils.hpp"

namespace Villain {

    Bone::Bone(const std::string& boneName, int ID, const aiNodeAnim* channel) :
        name(boneName), id(ID), localTransform(1.0f)
    {
        // Read and store animation keyframes for this bone from aiNodeAnim
        //printf("Animation for bone %s, Position keyframes: %i Rotation keyframes: %i Scaling keyframes: %i \n", boneName.c_str(), channel->mNumPositionKeys, channel->mNumRotationKeys, channel->mNumScalingKeys);
        numPositions = channel->mNumPositionKeys;
        for (int positionIndex = 0; positionIndex < numPositions; ++positionIndex) {
            aiVector3D position = channel->mPositionKeys[positionIndex].mValue;
            float timestep = channel->mPositionKeys[positionIndex].mTime;
            KeyPosition data;
            data.Position = AssimpUtils::aiVector3ToGLM(position);
            data.Timestamp = timestep;
            positions.push_back(data);
        }

        numRotations = channel->mNumRotationKeys;
        for (int rotationIndex = 0; rotationIndex < numRotations; ++rotationIndex) {
            aiQuaternion orientation = channel->mRotationKeys[rotationIndex].mValue;
            float timestep = channel->mRotationKeys[rotationIndex].mTime;
            KeyRotation data;
            data.Orientation = AssimpUtils::aiQuaternionToGLM(orientation);
            data.Timestamp = timestep;
            rotations.push_back(data);
        }

        numScales = channel->mNumScalingKeys;
        for (int scaleIndex = 0; scaleIndex < numScales; ++scaleIndex) {
            aiVector3D scaling = channel->mScalingKeys[scaleIndex].mValue;
            float timestep = channel->mScalingKeys[scaleIndex].mTime;
            KeyScale data;
            data.Scale = AssimpUtils::aiVector3ToGLM(scaling);
            data.Timestamp = timestep;
            scales.push_back(data);
        }

    }

    void Bone::update(float animationTime) {
        glm::mat4 translation = interpolatePosition(animationTime);
        glm::mat4 rotation = interpolateRotation(animationTime);
        glm::mat4 scale = interpolateScaling(animationTime);
        localTransform = translation * rotation * scale;
    }

    // Find keyframe in the node closest to specified animation time in ticks for interpolation
    int Bone::getPositionIndex(float animationTime) {
        for (int i = 0; i < numPositions - 1; ++i) {
            if (animationTime < positions[i + 1].Timestamp)
                return i;
        }
        return numPositions - 1;
    }

    int Bone::getRotationIndex(float animationTime) {
        for (int i = 0; i < numRotations - 1; ++i) {
            if (animationTime < rotations[i + 1].Timestamp)
                return i;
        }
        return numRotations - 1;

    }

    int Bone::getScaleIndex(float animationTime) {
        for (int i = 0; i < numScales - 1; ++i) {
            if (animationTime < scales[i + 1].Timestamp)
                return i;
        }
        return numScales - 1;

    }

    // Calculate normalised value for lerp and slerp
    float Bone::getScaleFactor(float lastTimestamp, float nextTimestamp, float animationTime) {
        float midwayLength = animationTime - lastTimestamp;
        float framesDiff = nextTimestamp - lastTimestamp;
        float factor = midwayLength / framesDiff;
        assert(factor >= 0.0f && factor <= 1.0f);
        return factor;
        //float delta = nextTimestamp - lastTimestamp;
        //return (animationTime - (float)nextTimestamp) / delta;
    }

    glm::mat4 Bone::interpolatePosition(float animationTime) {
        if (numPositions == 1)
            return glm::translate(glm::mat4(1.0f), positions[0].Position);

        int p0Index = getPositionIndex(animationTime);
        int p1Index = p0Index + 1;
        float scaleFactor = getScaleFactor(positions[p0Index].Timestamp, positions[p1Index].Timestamp, animationTime);
        glm::vec3 finalPosition = glm::mix(positions[p0Index].Position, positions[p1Index].Position, scaleFactor);
        return glm::translate(glm::mat4(1.0f), finalPosition);
    }

    glm::mat4 Bone::interpolateRotation(float animationTime) {
        if (numRotations == 1) {
            auto rotation = glm::normalize(rotations[0].Orientation);
            return glm::toMat4(rotation);
        }

        // To avoid gimbal-lock we use quaternions to represent rotations and thus we have to use
        // spherical linear interpolation(slerp) instead
        int r0Index = getRotationIndex(animationTime);
        int r1Index = r0Index + 1;
        float scaleFactor = getScaleFactor(rotations[r0Index].Timestamp, rotations[r1Index].Timestamp, animationTime);
        glm::quat finalRotation = glm::slerp(rotations[r0Index].Orientation, rotations[r1Index].Orientation, scaleFactor);
        return glm::toMat4(finalRotation);
    }

    glm::mat4 Bone::interpolateScaling(float animationTime) {
        if (numScales == 1)
            return glm::scale(glm::mat4(1.0f), scales[0].Scale);

        int s0Index = getScaleIndex(animationTime);
        int s1Index = s0Index + 1;
        float scaleFactor = getScaleFactor(scales[s0Index].Timestamp, scales[s1Index].Timestamp, animationTime);
        glm::vec3 finalScale = glm::mix(scales[s0Index].Scale, scales[s1Index].Scale, scaleFactor);
        return glm::scale(glm::mat4(1.0f), finalScale);
    }
}

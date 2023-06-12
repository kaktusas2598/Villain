#ifndef __BONE__
#define __BONE__

#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <list>
#include <vector>


namespace Villain {

    struct KeyPosition {
        glm::vec3 Position;
        float Timestamp;
    };

    struct KeyRotation {
        glm::quat Orientation;
        float Timestamp;
    };

    struct KeyScale {
        glm::vec3 Scale;
        float Timestamp;
    };

    // Single bone which reads all keyframes from aiNodeAnim and interpolates between position,rotation and scale based on time
    class Bone {
        public:
            Bone(const std::string& boneName, int ID, const aiNodeAnim* channel);

            void update(float animationTime);
            glm::mat4 getLocalTransform() const { return localTransform; }
            std::string getName() const { return name; }
            int getID() const { return id; }

            int getPositionIndex(float animationTime);
            int getRotationIndex(float animationTime);
            int getScaleIndex(float animationTime);

        private:
            float getScaleFactor(float lastTimestamp, float nextTimestamp, float animationTime);
            glm::mat4 interpolatePosition(float animationTime);
            glm::mat4 interpolateRotation(float animationTime);
            glm::mat4 interpolateScaling(float animationTime);

            std::vector<KeyPosition> positions;
            std::vector<KeyRotation> rotations;
            std::vector<KeyScale> scales;
            int numPositions, numRotations, numScales;

            glm::mat4 localTransform;
            std::string name;
            int id;
    };

}

#endif // __BONE__

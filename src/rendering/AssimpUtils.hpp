#pragma once

#include <assimp/matrix4x4.h>
#include <assimp/quaternion.h>
#include <assimp/vector3.h>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Villain {

    class AssimpUtils {
        public:

            static inline glm::mat4 aiMatrixToGLM(const aiMatrix4x4& from) {
                glm::mat4 to;
                // a,b,c and d in assimp are rows while 1,2,3 and 4 are columns
                to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
                to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
                to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
                to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
                return to;
            }

            static inline glm::vec3 aiVector3ToGLM(const aiVector3D& from) {
                return glm::vec3(from.x, from.y, from.z);
            }

            static inline glm::quat aiQuaternionToGLM(const aiQuaternion& from) {
                return glm::quat(from.w, from.x, from.y, from.z);
            }

    };
}

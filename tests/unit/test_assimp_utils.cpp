#include "catch2/catch_approx.hpp"
#include "catch2/catch_test_macros.hpp"

#include <assimp/matrix4x4.h>
#include <assimp/quaternion.h>
#include <assimp/vector3.h>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "rendering/AssimpUtils.hpp"

using namespace Villain;
using Catch::Approx;

TEST_CASE("AssimpUtils Test", "[AssimpUtils]") {
    SECTION("aiMatrixToGLM converts aiMatrix4x4 to glm::mat4") {
        // Create an aiMatrix4x4
        aiMatrix4x4 aiMatrix;
        aiMatrix.a1 = 1.0f; aiMatrix.a2 = 2.0f; aiMatrix.a3 = 3.0f; aiMatrix.a4 = 4.0f;
        aiMatrix.b1 = 5.0f; aiMatrix.b2 = 6.0f; aiMatrix.b3 = 7.0f; aiMatrix.b4 = 8.0f;
        aiMatrix.c1 = 9.0f; aiMatrix.c2 = 10.0f; aiMatrix.c3 = 11.0f; aiMatrix.c4 = 12.0f;
        aiMatrix.d1 = 13.0f; aiMatrix.d2 = 14.0f; aiMatrix.d3 = 15.0f; aiMatrix.d4 = 16.0f;

        // Convert aiMatrix4x4 to glm::mat4
        glm::mat4 glmMatrix = AssimpUtils::aiMatrixToGLM(aiMatrix);

        // Assert that the conversion is correct
        REQUIRE(glmMatrix[0][0] == Approx(1.0f));
        REQUIRE(glmMatrix[1][0] == Approx(2.0f));
        REQUIRE(glmMatrix[2][0] == Approx(3.0f));
        REQUIRE(glmMatrix[3][0] == Approx(4.0f));
        REQUIRE(glmMatrix[0][1] == Approx(5.0f));
        REQUIRE(glmMatrix[1][1] == Approx(6.0f));
        REQUIRE(glmMatrix[2][1] == Approx(7.0f));
        REQUIRE(glmMatrix[3][1] == Approx(8.0f));
        REQUIRE(glmMatrix[0][2] == Approx(9.0f));
        REQUIRE(glmMatrix[1][2] == Approx(10.0f));
        REQUIRE(glmMatrix[2][2] == Approx(11.0f));
        REQUIRE(glmMatrix[3][2] == Approx(12.0f));
        REQUIRE(glmMatrix[0][3] == Approx(13.0f));
        REQUIRE(glmMatrix[1][3] == Approx(14.0f));
        REQUIRE(glmMatrix[2][3] == Approx(15.0f));
        REQUIRE(glmMatrix[3][3] == Approx(16.0f));
    }

    SECTION("aiVector3ToGLM converts aiVector3D to glm::vec3") {
        // Create an aiVector3D
        aiVector3D aiVector(1.0f, 2.0f, 3.0f);

        // Convert aiVector3D to glm::vec3
        glm::vec3 glmVector = AssimpUtils::aiVector3ToGLM(aiVector);

        // Assert that the conversion is correct
        REQUIRE(glmVector.x == Approx(1.0f));
        REQUIRE(glmVector.y == Approx(2.0f));
        REQUIRE(glmVector.z == Approx(3.0f));
    }

    SECTION("aiQuaternionToGLM converts aiQuaternion to glm::quat") {
        // Create an aiQuaternion
        aiQuaternion aiQuat(0.5f, 0.1f, 0.2f, 0.3f);

        // Convert aiQuaternion to glm::quat
        glm::quat glmQuat = AssimpUtils::aiQuaternionToGLM(aiQuat);

        // Assert that the conversion is correct
        REQUIRE(glmQuat.w == Approx(0.5f));
        REQUIRE(glmQuat.x == Approx(0.1f));
        REQUIRE(glmQuat.y == Approx(0.2f));
        REQUIRE(glmQuat.z == Approx(0.3f));
    }
}

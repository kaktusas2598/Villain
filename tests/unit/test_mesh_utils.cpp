#include "catch2/catch_approx.hpp"
#include "catch2/catch_test_macros.hpp"

#include <glm/gtc/epsilon.hpp>
#include <glm/gtx/string_cast.hpp>

#include "rendering/MeshUtils.hpp"

using namespace Villain;

TEST_CASE("MeshUtils - Plane test", "[MeshUtils]") {
    // Create a vector to store the vertices and indices
    std::vector<VertexP1N1UV> vertices;
    std::vector<unsigned int> indices;
    glm::vec3 center(0.0f);
    glm::vec2 halfSize(0.5f);
    float uvCoords[4] = {0.0f, 1.0f, 0.0f, 1.0f};

    SECTION ("Generate XY Plane") {
        MeshUtils<VertexP1N1UV>::addXYPlane(&vertices, &indices, center, halfSize, uvCoords, false);

        REQUIRE(vertices.size() == 4);
        REQUIRE(indices.size() == 6);

        REQUIRE(vertices[0].Normal == glm::vec3(0.0f, 0.0f, -1.0f));
    }

    // Cleanup
    vertices.clear();
    indices.clear();

    SECTION("Generate XY Plane in opposite direction") {
        MeshUtils<VertexP1N1UV>::addXYPlane(&vertices, &indices, center, halfSize, uvCoords, true);
        REQUIRE(vertices[0].Normal == glm::vec3(0.0f, 0.0f, 1.0f));
    }


    // Cleanup
    vertices.clear();
    indices.clear();

    SECTION("Generate XZ Plane") {
        MeshUtils<VertexP1N1UV>::addXZPlane(&vertices, &indices, center, halfSize, uvCoords, false);

        REQUIRE(vertices.size() == 4);
        REQUIRE(indices.size() == 6);

        REQUIRE(vertices[0].Normal == glm::vec3(0.0f, 1.0f, 0.0f));
        REQUIRE(vertices[0].UV == glm::vec2(1.0f, 1.0f));
        REQUIRE(vertices[1].UV == glm::vec2(0.0f, 1.0f));
        REQUIRE(vertices[2].UV == glm::vec2(0.0f, 0.0f));
        REQUIRE(vertices[3].UV == glm::vec2(1.0f, 0.0f));
    }

    // Cleanup
    vertices.clear();
    indices.clear();

    SECTION("Generate YZ Plane") {
        MeshUtils<VertexP1N1UV>::addYZPlane(&vertices, &indices, center, halfSize, uvCoords, false);

        REQUIRE(vertices.size() == 4);
        REQUIRE(indices.size() == 6);

        REQUIRE(vertices[0].Normal == glm::vec3(-1.0f, 0.0f, 0.0f));
        REQUIRE(vertices[0].UV == glm::vec2(1.0f, 1.0f));
        REQUIRE(vertices[1].UV == glm::vec2(0.0f, 1.0f));
        REQUIRE(vertices[2].UV == glm::vec2(0.0f, 0.0f));
        REQUIRE(vertices[3].UV == glm::vec2(1.0f, 0.0f));
    }
}

TEST_CASE("MeshUtils - Tangents and Bitangents", "[MeshUtils]") {
    std::vector<VertexP1N1T1B1UV> vertices;
    std::vector<unsigned int> indices;

    SECTION("Generate Plane with Tangents and Bitangents") {
        glm::vec3 center(0.0f);
        glm::vec2 halfSize(0.5f);
        float uvCoords[4] = {0.0f, 1.0f, 0.0f, 1.0f};

        MeshUtils<VertexP1N1T1B1UV>::addXYPlane(&vertices, &indices, center, halfSize, uvCoords, false);
        MeshUtils<VertexP1N1T1B1UV>::addTangents(&vertices, &indices);

        REQUIRE(vertices.size() == 4);
        REQUIRE(indices.size() == 6);

        // Test tangent and bitangent calculations
        REQUIRE(vertices[0].Tangent == (glm::vec3(0.0f, -1.0f, 0.0f)));
        REQUIRE(vertices[0].BiTangent == (glm::vec3(-1.0f, 0.0f, 0.0f)));
    }
}

TEST_CASE("MeshUtils - Cube Generation", "[MeshUtils]") {
    std::vector<VertexP1N1UV> vertices;
    std::vector<unsigned int> indices;

    SECTION("Generate Cube") {
        glm::vec3 center(0.0f);
        glm::vec3 halfSize(0.5f);

        MeshUtils<VertexP1N1UV>::addAABB(&vertices, &indices, center, halfSize);

        // Test vertex and index counts
        REQUIRE(vertices.size() == 24);
        REQUIRE(indices.size() == 36);
    }
}

TEST_CASE("MeshUtils - Sphere Generation", "[MeshUtils]") {
    std::vector<VertexP1N1UV> vertices;
    std::vector<unsigned int> indices;

    SECTION("Generate Sphere") {
        glm::vec3 center(0.0f);
        float radius = 1.0f;
        int stacks = 4;
        int sectors = 8;

        MeshUtils<VertexP1N1UV>::addSphere(&vertices, &indices, radius, center, sectors, stacks);

        // Test vertex and index counts
        REQUIRE(vertices.size() == (stacks + 1) * (sectors + 1));
        REQUIRE(indices.size() == (stacks - 1) * sectors * 6);

        // TODO: add more assertions, normal checking, uv
    }
}

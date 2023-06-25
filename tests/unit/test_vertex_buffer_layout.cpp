#include "catch2/catch_approx.hpp"
#include "catch2/catch_test_macros.hpp"

#include "rendering/VertexBufferLayout.hpp"

TEST_CASE("VertexBufferLayout", "[VertexBufferLayout]") {
    SECTION("Pushing float elements") {
        VertexBufferLayout layout;
        layout.push<float>(3);
        layout.push<float>(2);

        const std::vector<VertexBufferElement>& elements = layout.getElements();

        REQUIRE(layout.getNumOfElements() == 2);
        REQUIRE(layout.getStride() == 20);

        REQUIRE(elements[0].type == GL_FLOAT);
        REQUIRE(elements[0].count == 3);
        REQUIRE(elements[0].normalised == GL_FALSE);

        REQUIRE(elements[1].type == GL_FLOAT);
        REQUIRE(elements[1].count == 2);
        REQUIRE(elements[1].normalised == GL_FALSE);
    }

    SECTION("Pushing unsigned int elements") {
        VertexBufferLayout layout;
        layout.push<unsigned int>(1);
        layout.push<unsigned int>(4);

        const std::vector<VertexBufferElement>& elements = layout.getElements();

        REQUIRE(layout.getNumOfElements() == 2);
        REQUIRE(layout.getStride() == 20);

        REQUIRE(elements[0].type == GL_UNSIGNED_INT);
        REQUIRE(elements[0].count == 1);
        REQUIRE(elements[0].normalised == GL_FALSE);

        REQUIRE(elements[1].type == GL_UNSIGNED_INT);
        REQUIRE(elements[1].count == 4);
        REQUIRE(elements[1].normalised == GL_FALSE);
    }

    SECTION("Pushing int elements") {
        VertexBufferLayout layout;
        layout.push<int>(2);
        layout.push<int>(3);

        const std::vector<VertexBufferElement>& elements = layout.getElements();

        REQUIRE(layout.getNumOfElements() == 2);
        REQUIRE(layout.getStride() == 20);

        REQUIRE(elements[0].type == GL_INT);
        REQUIRE(elements[0].count == 2);
        REQUIRE(elements[0].normalised == GL_FALSE);

        REQUIRE(elements[1].type == GL_INT);
        REQUIRE(elements[1].count == 3);
        REQUIRE(elements[1].normalised == GL_FALSE);
    }

    SECTION("Pushing unsigned char elements") {
        VertexBufferLayout layout;
        layout.push<unsigned char>(3);
        layout.push<unsigned char>(1);

        const std::vector<VertexBufferElement>& elements = layout.getElements();

        REQUIRE(layout.getNumOfElements() == 2);
        REQUIRE(layout.getStride() == 4);

        REQUIRE(elements[0].type == GL_UNSIGNED_BYTE);
        REQUIRE(elements[0].count == 3);
        REQUIRE(elements[0].normalised == GL_TRUE);

        REQUIRE(elements[1].type == GL_UNSIGNED_BYTE);
        REQUIRE(elements[1].count == 1);
        REQUIRE(elements[1].normalised == GL_TRUE);
    }
}

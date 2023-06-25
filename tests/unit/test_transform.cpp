#include "catch2/catch_approx.hpp"
#include "catch2/catch_test_macros.hpp"

// Include the necessary header for the epsilon comparison
#include <glm/gtc/epsilon.hpp>

#include "Transform.hpp"

TEST_CASE("Transform class tests") {
    SECTION("Default construction") {
        Villain::Transform transform;

        // Test default values
        REQUIRE(transform.getPos() == glm::vec3(0.0f));
        REQUIRE(transform.getEulerRot() == glm::vec3(0.0f));
        REQUIRE(transform.getScale() == 1.0f);
    }

    SECTION("Translation") {
        Villain::Transform transform;

        // Test translation
        transform.translatePosition(glm::vec3(1.0f, 2.0f, 3.0f));
        REQUIRE(transform.getPos() == glm::vec3(1.0f, 2.0f, 3.0f));

        transform.setPos(glm::vec3(0.0f));
        REQUIRE(transform.getPos() == glm::vec3(0.0f));
    }

    SECTION("Rotation") {
        Villain::Transform transform;

        // Test rotation
        transform.setEulerRot(45.0f, 0.0f, 90.0f);
        REQUIRE(transform.getEulerRot() == glm::vec3(45.0f, 0.0f, 90.0f));

        REQUIRE(transform.getForward().x == Catch::Approx(0.707107));
        REQUIRE(transform.getForward().y == Catch::Approx(0.707107));
        REQUIRE(transform.getForward().z == Catch::Approx(0.0));

        REQUIRE(transform.getBackward().x == Catch::Approx(-0.707107));
        REQUIRE(transform.getBackward().y == Catch::Approx(-0.707107));
        REQUIRE(transform.getBackward().z == Catch::Approx(0.0));

        REQUIRE(transform.getRight().x == Catch::Approx(0.0));
        REQUIRE(transform.getRight().y == Catch::Approx(0.0));
        REQUIRE(transform.getRight().z == Catch::Approx(1.0));

        REQUIRE(transform.getUp().x == Catch::Approx(-0.707107));
        REQUIRE(transform.getUp().y == Catch::Approx(0.707107));
        REQUIRE(transform.getUp().z == Catch::Approx(0.0));

    }

    SECTION("Scaling") {
        Villain::Transform transform;

        // Test scaling
        transform.setScale(2.0f);
        REQUIRE(transform.getScale() == 2.0f);
        REQUIRE(*transform.getScalePtr() == 2.0f);
    }
}


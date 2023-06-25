#include "catch2/catch_approx.hpp"
#include "catch2/catch_test_macros.hpp"

// Include the necessary header for the epsilon comparison
#include <glm/gtc/epsilon.hpp>

#include "Camera.hpp"

TEST_CASE("Perspective Camera Test", "[Camera]") {
    // Create a camera object for testing
    Villain::Camera camera(Villain::ProjectionType::PERSPECTIVE);

    SECTION("Default Camera Values") {
        // Check the default values of the camera
        REQUIRE(camera.getPosition() == glm::vec3(0.0f, 0.0f, 1.0f));
        REQUIRE(camera.getZoom() == 45.0f);
        REQUIRE(camera.getZnear() == Catch::Approx(0.1f));
        REQUIRE(camera.getZfar() == Catch::Approx(100.0f));
        REQUIRE(camera.getProjectionType() == Villain::ProjectionType::PERSPECTIVE);
    }

    SECTION("Camera Aspect Ratio") {
        camera.rescale(100.f, 100.f);
        REQUIRE(camera.getAspectRatio() == Catch::Approx(1.0f)); // Assuming square aspect ratio
    }

    SECTION("Camera Keyboard Movement") {
        // Test camera movement
        //printf("Epsilon used: %f\n", glm::epsilon<float>());
        camera.processKeyboard(Villain::CameraMovement::FORWARD, 0.1f);
        // Use the epsilon comparison function to check if the values are close enough
        REQUIRE(glm::all(glm::epsilonEqual(camera.getPosition(), glm::vec3(0.0, 0.0, 0.75), glm::epsilon<float>())));

        camera.processKeyboard(Villain::CameraMovement::BACKWARD, 0.1f);
        REQUIRE(glm::all(glm::epsilonEqual(camera.getPosition(), glm::vec3(0.0, 0.0f, 1.0), glm::epsilon<float>())));

        camera.processKeyboard(Villain::CameraMovement::LEFT, 0.1f);
        REQUIRE(glm::all(glm::epsilonEqual(camera.getPosition(), glm::vec3(-0.25, 0.0, 1.0), glm::epsilon<float>())));

        camera.processKeyboard(Villain::CameraMovement::RIGHT, 0.1f);
        REQUIRE(glm::all(glm::epsilonEqual(camera.getPosition(), glm::vec3(0.0, 0.0, 1.0), glm::epsilon<float>())));

        camera.processKeyboard(Villain::CameraMovement::UP, 0.1f);
        REQUIRE(glm::all(glm::epsilonEqual(camera.getPosition(), glm::vec3(0.0, 0.25, 1.0), glm::epsilon<float>())));

        camera.processKeyboard(Villain::CameraMovement::DOWN, 0.1f);
        REQUIRE(glm::all(glm::epsilonEqual(camera.getPosition(), glm::vec3(0.0, 0.0, 1.0), glm::epsilon<float>())));

    }

    SECTION("Camera Mouse Movement") {
        camera.processMouseMovement(1.0f, 1.0f);
        REQUIRE(camera.getYaw() == -89.9f);
        REQUIRE(camera.getPitch() == 0.1f);

        camera.processMouseScroll(45.0f);
        REQUIRE(camera.getZoom() == 1.0f);

        camera.processMouseScroll(-45.0f);
        REQUIRE(camera.getZoom() == 45.0f);

        camera.setZoom(0.0f);
        REQUIRE(camera.getZoom() == 1.0f);

        camera.setZoom(46.0f);
        REQUIRE(camera.getZoom() == 45.0f);
    }

    SECTION("Camera Manipulation") {
        camera.setRotation(glm::vec3(45.f, 45.f, 45.f));

        REQUIRE(camera.getYaw() == 45.f);
        REQUIRE(camera.getPitch() == 45.f);
        REQUIRE(camera.getRoll() == 45.f);

        REQUIRE(glm::all(glm::epsilonEqual(camera.getRotation(), glm::vec3(45.0, 45.0, 45.0), glm::epsilon<float>())));

        camera.setPosition({0, 0, 0});
        camera.offsetPosition({1, 1});
        REQUIRE(camera.getPosition().x == 1.f);
        REQUIRE(camera.getPosition().y == 1.f);

        camera.offsetScale(10.f);
        REQUIRE(camera.getZoom() == 55.f);
    }

    SECTION("Camera View Matrix") {
        REQUIRE(camera.getViewMatrix() == glm::lookAt(camera.getPosition(), camera.getPosition() + camera.getFront(), camera.getUp()));
    }

}


TEST_CASE("Raycasting Test", "[Camera]") {
    Villain::Camera camera(Villain::ProjectionType::PERSPECTIVE);
    camera.setPosition(glm::vec3(0.0f, 0.0f, 1.0f));
    // Face toward negative z direction
    camera.setRotation(glm::vec3(0.0f, -90.0f, 0.0f));
    camera.rescale(800, 600);

    SECTION("Test conversion of mouse coordinates to world ray") {
        glm::vec2 mouseCoords(400.0f, 300.0f);
        glm::vec3 expectedRay(0.0f, 0.0f, -1.0f);

        glm::vec3 result = camera.mouseRayToWorld(mouseCoords);

        REQUIRE(glm::all(glm::epsilonEqual(result, expectedRay, glm::epsilon<float>())));
    }
}

TEST_CASE("Camera Frustum Test", "[Camera]") {
    Villain::Camera camera(Villain::ProjectionType::PERSPECTIVE);
    camera.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    camera.setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    camera.rescale(800, 600);

    SECTION("Test getting the frustum of the camera") {
        Villain::Frustum expected{
            Villain::Plane({0.382683f, -0.923880f, 1.0f}, 1.0f),
            Villain::Plane({1.0f, 1.0f, 1.0f}, 1.0f),
            Villain::Plane({1.0f, 1.0f, 1.0f}, 1.0f),
            Villain::Plane({1.0f, 1.0f, 1.0f}, 1.0f),
            Villain::Plane({1.0f, 0.0f, 0.0f}, 0.1f),
            Villain::Plane({-1.0f, 0.0f, 0.0f}, -100.0f)
        };

        Villain::Frustum result = camera.getFrustum();

        // Compare each plane of the frustum
        //printf("%f %f %f\n", result.bottomFace.getNormal().x, result.bottomFace.getNormal().y, result.bottomFace.getNormal().z);
        REQUIRE(result.nearFace.getDistance() == expected.nearFace.getDistance());
        REQUIRE(result.nearFace.getNormal() == expected.nearFace.getNormal());

        REQUIRE(result.farFace.getDistance() == expected.farFace.getDistance());
        REQUIRE(result.farFace.getNormal() == expected.farFace.getNormal());
    }

    SECTION("Unsupported projections for frustum") {
        camera.setProjectionType(Villain::ProjectionType::ORTHOGRAPHIC_2D);

        Villain::Frustum expected;
        Villain::Frustum result = camera.getFrustum();

        REQUIRE(result.nearFace.getDistance() == expected.nearFace.getDistance());
        REQUIRE(result.nearFace.getNormal() == expected.nearFace.getNormal());

        REQUIRE(result.leftFace.getDistance() == expected.leftFace.getDistance());
        REQUIRE(result.leftFace.getNormal() == expected.leftFace.getNormal());

    }
}

TEST_CASE("2D Camera Test", "[Camera]") {
    Villain::Camera camera(Villain::ProjectionType::ORTHOGRAPHIC_2D);
    camera.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    camera.setZoom(1.0f);
    camera.rescale(800, 600);

    SECTION("2D Culling") {
        glm::vec2 quadPosition(0.0f, 0.0f);
        glm::vec2 quadDimensions(100.0f, 100.0f);

        bool result = camera.quadInView(quadPosition, quadDimensions);

        REQUIRE(result == true);
    }

    SECTION("Screen to World Coordinate conversion") {
        glm::vec2 screenCoords(400.0f, 300.0f);
        glm::vec2 expectedWorldCoords(0.0f, 0.0f);

        glm::vec2 result = camera.screenToWorld(screenCoords);

        REQUIRE(result == expectedWorldCoords);
    }
}
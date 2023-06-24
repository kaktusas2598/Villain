#include "catch2/catch_approx.hpp"
#include "catch2/catch_test_macros.hpp"

// Include the necessary header for the epsilon comparison
#include <glm/gtc/epsilon.hpp>

#include "Camera.hpp"

TEST_CASE("Camera Test") {
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

    SECTION("Camera Size") {
        camera.rescale(100.f, 100.f);


        REQUIRE(camera.getAspectRatio() == Catch::Approx(1.0f)); // Assuming square aspect ratio
    }

    SECTION("Camera Keyboard Movement") {
        // Test camera movement
        printf("Epsilon used: %f\n", glm::epsilon<float>());
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
    }

    // ... add more test sections for other camera functionalities
}


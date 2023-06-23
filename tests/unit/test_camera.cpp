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
        //REQUIRE(camera.getAspectRatio() == Approx(1.0f)); // Assuming square aspect ratio
        //REQUIRE(camera.getZnear() == Approx(0.1f));
        //REQUIRE(camera.getZfar() == Approx(100.0f));
        REQUIRE(camera.getProjectionType() == Villain::ProjectionType::PERSPECTIVE);
    }

    SECTION("Camera Movement") {
        // Test camera movement
        printf("Epsilon used: %f\n", glm::epsilon<float>());
        camera.processKeyboard(Villain::CameraMovement::FORWARD, 0.1f);
        // Use the epsilon comparison function to check if the values are close enough
        REQUIRE(glm::all(glm::epsilonEqual(camera.getPosition(), glm::vec3(0.0f, 0.0f, 0.75f), glm::epsilon<float>())));

        camera.processKeyboard(Villain::CameraMovement::LEFT, 0.1f);
        REQUIRE(glm::all(glm::epsilonEqual(camera.getPosition(), glm::vec3(-0.25, 0.0f, 0.75f), glm::epsilon<float>())));

        camera.processKeyboard(Villain::CameraMovement::UP, 0.1f);
        REQUIRE(glm::all(glm::epsilonEqual(camera.getPosition(), glm::vec3(-0.25, 0.25, 0.75f), glm::epsilon<float>())));

        // ... add more tests for different camera movements
    }

    // ... add more test sections for other camera functionalities
}


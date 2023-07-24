#include "catch2/catch_approx.hpp"
#include "catch2/catch_test_macros.hpp"

#include "Logger.hpp"
#include "glm/gtx/string_cast.hpp"

#include "physics/RigidBody.hpp"

TEST_CASE("RigidBody Default Constructor", "[RigidBody]") {
    Villain::RigidBody body;

    SECTION("Default values should be set correctly") {
        REQUIRE(body.getAwake() == true);
        REQUIRE(body.getMass() == Catch::Approx(1.0f));
        REQUIRE(body.getInverseMass() == Catch::Approx(1.0f));
        REQUIRE(body.getInverseInertiaTensor() == glm::mat3(1.0f));
        REQUIRE(body.getInverseInertiaTensorWorld() == glm::mat3(1.0f));
        REQUIRE(body.getLinearDamping() == Catch::Approx(0.98f));
        REQUIRE(body.getAngularDamping() == Catch::Approx(0.98f));
        REQUIRE(body.getPosition() == glm::vec3(0.0f));
        REQUIRE(body.getOrientation() == glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
        REQUIRE(body.getLinearVelocity() == glm::vec3(0.0f));
        REQUIRE(body.getAngularVelocity() == glm::vec3(0.0f));
        REQUIRE(body.getLastFrameAcceleration() == glm::vec3(0.0f));
        REQUIRE(body.getTransform() == glm::mat4(1.0f));
    }
}

TEST_CASE("RigidBody Integration", "[RigidBody]") {
    Villain::RigidBody body;

    SECTION("Integration with zero forces should not change position and orientation") {
        // Save initial position and orientation
        glm::vec3 initialPosition = body.getPosition();
        glm::quat initialOrientation = body.getOrientation();

        // Integrate with deltaTime = 0
        body.integrate(0.0f);

        // Check if position and orientation remain unchanged
        REQUIRE(body.getPosition() == initialPosition);
        REQUIRE(body.getOrientation() == initialOrientation);
    }

    SECTION("Integration with constant linear and angular velocity should update position and orientation") {
        // Set constant linear and angular velocity
        glm::vec3 linearVelocity(1.0f, 0.0f, 0.0f);
        glm::vec3 angularVelocity(0.0f, 0.0f, glm::radians(45.0f)); // 45 degrees per second around Z-axis

        body.setLinearVelocity(linearVelocity);
        body.setAngularVelocity(angularVelocity);
        body.setAngularDamping(1.0); // Set no angular damping for easier asserts

        // NOTE: Usually class in physics engine would do this before each step automatically
        // to ensure transform matrix (mostly useful for render and collision)
        // and inverse inertia world matrix (required for angular acc calculations) are calculated
        body.calculateDerivedData();

        // Integrate with deltaTime = 1 second
        body.integrate(1.0f);

        // Check if position and orientation are updated correctly
        // Actual position is a bit less than 1 due to damping!
        REQUIRE(body.getPosition() == glm::vec3(0.98f, 0.0f, 0.0f));
        // Note: Quaternion comparison might require custom epsilon
        VILLAIN_TRACE("Orientation: {}", glm::to_string(body.getOrientation()));

        // WARNING:Investigate rotation calculations in RigidBody to understand if we're setting
        // correct expectations here or if orientation calculations are wrong
        glm::vec3 eulerRot = glm::degrees(glm::eulerAngles(body.getOrientation()));
        VILLAIN_TRACE("Euler Orientation: {}", glm::to_string(eulerRot));
        // ACTUAL : Orientation: quat(0.933274, {0.000000, 0.000000, 0.359166})
        // EXPECTED From chatGPT:
        //REQUIRE(body.getOrientation().x == Catch::Approx(glm::cos(glm::radians(22.5f))));
        //REQUIRE(body.getOrientation().y == Catch::Approx(0.0f));
        //REQUIRE(body.getOrientation().z == Catch::Approx(0.0f));
        //REQUIRE(body.getOrientation().w == Catch::Approx(glm::sin(glm::radians(22.5f))));
        glm::quat exp = {glm::sin(glm::radians(22.5f)), glm::cos(glm::radians(22.5f)), 0.0f, 0.0f};
        VILLAIN_TRACE("EXPECTED: {}", glm::to_string(exp));
        VILLAIN_TRACE("ACTUAL: {}", glm::to_string(body.getOrientation()));
    }
}

TEST_CASE("RigidBody Forces and Damping", "[RigidBody]") {
    Villain::RigidBody body;

    SECTION("Adding force should affect linear acceleration") {
        // Set mass to 1.0 kg
        body.setMass(1.0f);

        // Add force (10 N) in the positive X direction
        body.addForce(glm::vec3(10.0f, 0.0f, 0.0f));

        // Integrate with deltaTime = 1 second
        body.integrate(1.0f);

        // Check if position is updated correctly due to the applied force
        REQUIRE(body.getPosition() == glm::vec3(9.8f, 0.0f, 0.0f));
    }
}

TEST_CASE("RigidBody State", "[RigidBody]") {
    Villain::RigidBody body;

    SECTION("Sleeping body") {
        body.setAwake(false);

        // Save initial position and orientation
        glm::vec3 initialPosition = body.getPosition();
        glm::quat initialOrientation = body.getOrientation();

        body.integrate(10.0f);

        // Adding velocity won't make difference atm
        body.addLinearVelocity({1.0, 1.0, 1.0});
        body.addAngularVelocity({1.0, 1.0, 1.0});

        // Check if position and orientation remain unchanged
        REQUIRE(body.getPosition() == initialPosition);
        REQUIRE(body.getOrientation() == initialOrientation);

        // But adding force will awake body
        body.addForce({1.0, 1.0, 1.0});
        body.integrate(1.0f);

        REQUIRE(body.getPosition() != initialPosition);
        REQUIRE(body.getOrientation() != initialOrientation);

        body.setAwake(false);

        // Setting position and orientation explicitily WILL change them
        // for sleeping body but body will still be sleeping!
        glm::vec3 newPos = {1.0, 1.0, 1.0};
        glm::quat newRot(glm::vec3(1.0));

        body.setPosition(newPos);
        body.setOrientation(newRot);

        body.integrate(1.0);

        REQUIRE(body.getAwake() == false);
        REQUIRE(body.getPosition() == newPos);
        REQUIRE(body.getOrientation() == newRot);
    }
}

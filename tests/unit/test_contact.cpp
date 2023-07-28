#include "Logger.hpp"
#include "catch2/catch_approx.hpp"
#include "catch2/catch_test_macros.hpp"

#include "glm/gtx/string_cast.hpp"
#include "physics/Contact.hpp"

TEST_CASE("Contact Default Constructor") {
    Villain::Contact contact;
    REQUIRE(contact.getBody(0) == nullptr);
    REQUIRE(contact.getBody(1) == nullptr);
    REQUIRE(contact.getFriction() == Catch::Approx(-1.0f));
    REQUIRE(contact.getRestitution() == Catch::Approx(-1.0f));
    REQUIRE(contact.getContactPoint() == glm::vec3(-9999.0f));
    REQUIRE(contact.getContactNormal() == glm::vec3(0.0f));
    REQUIRE(contact.getPenetration() == Catch::Approx(-9999.0f));
    REQUIRE(contact.getContactToWorld() == glm::mat3(0.0f));
    REQUIRE(contact.getContactVelocity() == glm::vec3(0.0f));
    REQUIRE(contact.getDesiredDeltaVelocity() == Catch::Approx(0.0f));
    REQUIRE(contact.getRelativeContactPos(0) == glm::vec3(-9999.0f));
    REQUIRE(contact.getRelativeContactPos(1) == glm::vec3(-9999.0f));
}

TEST_CASE("Collision Response Tests") {
    const float epsilon = 0.001f; // A small value for floating-point comparisons

    SECTION("Invalid contact so no collision response") {
        Villain::RigidBody body1;
        Villain::RigidBody body2;
        // By default contact will be invalid and there will be no collision
        Villain::Contact contact;

        // Set up bodies and collision
        body1.setMass(1.0f);
        body1.setInertiaTensor(glm::mat3(1.0f));
        body1.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
        body1.calculateDerivedData();

        body2.setMass(1.0f);
        body2.setInertiaTensor(glm::mat3(1.0f));
        body2.setPosition(glm::vec3(1.0f, 0.0f, 0.0f));
        body2.calculateDerivedData();

        contact.setBodyData(&body1, &body2, 0.0f, 0.0f);

        // Resolve collision
        Villain::ContactResolver resolver{1};
        resolver.resolveContacts(&contact, 1, 0.1f);

        // Verify expected outcomes
        REQUIRE(body1.getLinearVelocity() == glm::vec3(0.0f));
        REQUIRE(body1.getAngularVelocity() == glm::vec3(0.0f));
        REQUIRE(body2.getLinearVelocity() == glm::vec3(0.0f));
        REQUIRE(body2.getAngularVelocity() == glm::vec3(0.0f));
    }

    SECTION("Collision between two stationary bodies with no friction") {
        Villain::RigidBody body1;
        Villain::RigidBody body2;
        Villain::Contact contact;

        // Set up bodies and collision
        body1.setMass(1.0f);
        body1.setInertiaTensor(glm::mat3(1.0f));
        body1.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
        body1.calculateDerivedData();

        body2.setMass(1.0f);
        body2.setInertiaTensor(glm::mat3(1.0f));
        body2.setPosition(glm::vec3(1.0f, 0.0f, 0.0f));
        body2.calculateDerivedData();

        // Set up contact data
        glm::vec3 contactNormal = glm::normalize(body2.getPosition() - body1.getPosition());
        glm::vec3 contactPoint = (body1.getPosition() + body2.getPosition()) * 0.5f;
        float penetration = glm::distance(body1.getPosition(), body2.getPosition()) - 1.0f; // Assuming the bodies have a radius of 1.0f
        float friction = 0.0f, restitution = 0.0f;

        contact.setBodyData(&body1, &body2, friction, restitution);
        contact.setPenetration(penetration);
        contact.setContactNormal(contactNormal);
        contact.setContactPoint(contactPoint);

        // Resolve collision
        Villain::ContactResolver resolver{1};
        resolver.resolveContacts(&contact, 1, 0.1f);

        // WARNING:: Need to come up with correct expectations here for a frictionless contact response
        // Verify expected outcomes
        glm::vec3 expectedLinearVelocity1 = glm::vec3(0.5f, 0.0f, 0.0f); // Half of the separation in one time step
        glm::vec3 expectedLinearVelocity2 = glm::vec3(-0.5f, 0.0f, 0.0f); // Half of the separation in one time step

        VILLAIN_TRACE("ACTUAL1 LIN VEL.: {}", glm::to_string(body1.getLinearVelocity()));
        VILLAIN_TRACE("ACTUAL2 LIN VEL.: {}", glm::to_string(body2.getLinearVelocity()));
        //REQUIRE(glm::distance(body1.getLinearVelocity(), expectedLinearVelocity1) < epsilon);
        //REQUIRE(glm::distance(body2.getLinearVelocity(), expectedLinearVelocity2) < epsilon);

        // Angular velocity should still be zero since the bodies have no initial rotation
        REQUIRE(body1.getAngularVelocity() == glm::vec3(0.0f));
        REQUIRE(body2.getAngularVelocity() == glm::vec3(0.0f));
    }
}

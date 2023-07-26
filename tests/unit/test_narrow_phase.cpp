#include "catch2/catch_approx.hpp"
#include "catch2/catch_test_macros.hpp"

#include "Logger.hpp"
#include "glm/gtx/string_cast.hpp"
#include "physics/NarrowPhase.hpp"

TEST_CASE("CollisionDetector - Sphere and Sphere", "[CollisionDetector]") {
    Villain::CollisionData data(new Villain::Contact[4], 4, 0, 0.0f, 0.0f);

    Villain::RigidBody body1;
    Villain::CollisionSphere sphere1(1.0f, &body1);

    Villain::RigidBody body2;
    Villain::CollisionSphere sphere2(1.0f, &body2);

    // Test non-intersecting spheres
    body2.setPosition({3.0, 0.0, 0.0});
    body2.calculateDerivedData();
    sphere2.calculateTransform();
    REQUIRE(Villain::CollisionDetector::sphereAndSphere(sphere1, sphere2, &data) == 0);

    // Test intersecting spheres
    body2.setPosition({1.5, 0.0, 0.0});
    body2.calculateDerivedData();
    sphere2.calculateTransform();
    REQUIRE(Villain::CollisionDetector::sphereAndSphere(sphere1, sphere2, &data) == 1);
    REQUIRE(data.contactCount == 1);

    data.contacts -= 1; // Decrease contact pointer because collision data owns it
    REQUIRE(data.contacts[0].penetration == Catch::Approx(0.5f));
    REQUIRE(data.contacts[0].contactNormal == glm::vec3(-1.0, 0.0, 0.0));
}

TEST_CASE("CollisionDetector - Sphere and HalfSpace", "[CollisionDetector]") {
    Villain::CollisionData data(new Villain::Contact[4], 4, 0, 0.0f, 0.0f);

    // Test non-intersecting sphere and half-space
    Villain::RigidBody body1;
    body1.setPosition({0.0, 2.0, 0.0});
    body1.calculateDerivedData();
    Villain::CollisionSphere sphere(1.0f, &body1);
    sphere.calculateTransform();

    Villain::CollisionPlane plane(glm::vec3(0.0f, 1.0f, 0.0f), 0.0f);

    REQUIRE(Villain::CollisionDetector::sphereAndHalfSpace(sphere, plane, &data) == 0);

    // Test intersecting sphere and half-space
    body1.setPosition({0.0, 0.0, 0.0});
    body1.calculateDerivedData();
    sphere.calculateTransform();
    REQUIRE(Villain::CollisionDetector::sphereAndHalfSpace(sphere, plane, &data) == 1);
    REQUIRE(data.contactCount == 1);

    data.contacts -= 1; // Decrease contact pointer because collision data owns it
    REQUIRE(data.contacts[0].penetration == Catch::Approx(1.0f));
    REQUIRE(data.contacts[0].contactNormal == glm::vec3(0.0, 1.0, 0.0));
}

TEST_CASE("CollisionDetector - Box and HalfSpace", "[CollisionDetector]") {
    Villain::CollisionData data(new Villain::Contact[16], 16, 0, 0.0f, 0.0f);
    Villain::RigidBody body1;
    // XZ/Ground plane at y=0
    Villain::CollisionPlane plane(glm::vec3(0.0f, 1.0f, 0.0f), 0.0f);
    // 1x1x1 box
    Villain::CollisionBox box({0.5, 0.5, 0.5}, &body1);

    SECTION("Box and HalfSpace are not intersecting so no contacts are generated") {
        body1.setPosition({0.0, 2.0, 0.0});
        body1.calculateDerivedData();
        box.calculateTransform();
        REQUIRE(Villain::CollisionDetector::boxAndHalfSpace(box, plane, &data) == 0);
    }

    // Bottom face of the box intersects with plane, so 4 contacts
    SECTION("Box and HalfSpace are intersecting 4 contacts generated") {
        body1.setPosition({0.0, 0.0, 0.0});
        body1.calculateDerivedData();
        box.calculateTransform();
        REQUIRE(Villain::CollisionDetector::boxAndHalfSpace(box, plane, &data) == 4);
        REQUIRE(data.contactCount == 4);

        data.contacts -= 4; // Decrease contact pointer because collision data owns it
        // 1st contact
        // WARNING: Not sure if y coordinates for contactPints make sense here
        REQUIRE(data.contacts[0].penetration == Catch::Approx(0.5f));
        REQUIRE(data.contacts[0].contactNormal == glm::vec3(0.0, 1.0, 0.0));
        REQUIRE(data.contacts[0].contactPoint == glm::vec3(0.5f, -1.0f, 0.5f));
        //// 2nd contact
        REQUIRE(data.contacts[1].penetration == Catch::Approx(0.5f));
        REQUIRE(data.contacts[1].contactNormal == glm::vec3(0.0, 1.0, 0.0));
        REQUIRE(data.contacts[1].contactPoint == glm::vec3(-0.5f, -1.0f, 0.5f));
        //// 3rd contact
        REQUIRE(data.contacts[2].penetration == Catch::Approx(0.5f));
        REQUIRE(data.contacts[2].contactNormal == glm::vec3(0.0, 1.0, 0.0));
        REQUIRE(data.contacts[2].contactPoint == glm::vec3(0.5f, -1.0f, -0.5f));
        //// 4th contact
        REQUIRE(data.contacts[3].penetration == Catch::Approx(0.5f));
        REQUIRE(data.contacts[3].contactNormal == glm::vec3(0.0, 1.0, 0.0));
        REQUIRE(data.contacts[3].contactPoint == glm::vec3(-0.5f, -1.0f, -0.5f));
    }

    // TODO:
    SECTION("Box and HalfSpace are intersecting 3 contacts generated") {
        body1.setPosition({0.0, 0.0, 0.0});
        body1.setOrientation(glm::angleAxis(45.0f, glm::vec3(1.0, 0.0, 0.0)));
        body1.calculateDerivedData();
        box.calculateTransform();
        // FIXME: 4 contacts are generated here? Engine's fault or wrong input data?
        //REQUIRE(Villain::CollisionDetector::boxAndHalfSpace(box, plane, &data) == 3);
        //REQUIRE(data.contactCount == 3);
        data.contacts -= data.contactCount; // Decrease contact pointer because collision data owns it
        VILLAIN_TRACE("ACTUAL0 NORMAL: {}", glm::to_string(data.contacts[0].contactNormal));
        VILLAIN_TRACE("ACTUAL0 POINT: {}", glm::to_string(data.contacts[0].contactPoint));
        VILLAIN_TRACE("ACTUAL0 PENETRATION: {}", data.contacts[0].penetration);
    }

    SECTION("Box and HalfSpace are intersecting 2 contacts generated") {
        // TODO: Same idea as with 4 contact section above
    }

    SECTION("Box and HalfSpace are intersecting 1 contact generated") {
        // TODO: Same idea as with 4 contact section above
    }
}

TEST_CASE("CollisionDetector - Box and Sphere", "[CollisionDetector]") {
    Villain::CollisionData data(new Villain::Contact[16], 4, 0, 0.0f, 0.0f);

    Villain::RigidBody body1, body2;
    // 1x1x1 box
    Villain::CollisionBox box({0.5, 0.5, 0.5}, &body1);
    // 0.5 radius sphere
    Villain::CollisionSphere sphere(0.5f, &body2);

    SECTION("Box and Sphere are not intersecting") {
        body1.setPosition({2.0, 0.0, 0.0});
        body1.calculateDerivedData();
        box.calculateTransform();

        REQUIRE(Villain::CollisionDetector::boxAndSphere(box, sphere, &data) == 0);
    }

    SECTION("Box and Sphere are intersecting") {
        body1.setPosition({1.0, 0.0, 0.0});
        body1.calculateDerivedData();
        box.calculateTransform();

        REQUIRE(Villain::CollisionDetector::boxAndSphere(box, sphere, &data) == 1);
    }
    // TODO: More test cases
}

TEST_CASE("CollisionDetector - Box and Box", "[CollisionDetector]") {
    Villain::CollisionData data(new Villain::Contact[16], 4, 0, 0.0f, 0.0f);

    Villain::RigidBody body1, body2;
    // 2 1x1x1 boxes
    Villain::CollisionBox box({0.5, 0.5, 0.5}, &body1);
    Villain::CollisionBox box2({0.5, 0.5, 0.5}, &body2);

    SECTION("Box and Sphere are not intersecting") {
        body1.setPosition({2.0, 0.0, 0.0});
        body1.calculateDerivedData();
        box.calculateTransform();

        REQUIRE(Villain::CollisionDetector::boxAndBox(box, box2, &data) == 0);
    }

    // TODO: test
    //SECTION("Box and Sphere are intersecting") {
        //body1.setPosition({1.0, 0.0, 0.0});
        //body1.calculateDerivedData();
        //box.calculateTransform();

        //REQUIRE(Villain::CollisionDetector::boxAndBox(box, box2, &data) == 1);
    //}
}

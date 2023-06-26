#include "catch2/catch_approx.hpp"
#include "catch2/catch_test_macros.hpp"

#include "physics/BoundingSphere.hpp"
#include "physics/BoundingAABB.hpp"

using namespace Villain;

// Mock Collider type to test unsupported collider/bounding volume types
class FakeCollider : public Collider {
    public:
        FakeCollider() : Collider((ColliderType)100000) {}

        virtual const glm::vec3& getCentre() const { return centre; }
        virtual void render(DebugRenderer* renderer) {}
    private:
        glm::vec3 centre;
};

TEST_CASE("BoundingSphere intersecting BoundingSphere", "[BoundingSphere]") {
    BoundingSphere sphere1(glm::vec3(0.0f), 1.0f);
    BoundingSphere sphere2(glm::vec3(1.0f, 0.0f, 0.0f), 1.0f);


    REQUIRE(sphere1.getRadius() == 1.0f);
    REQUIRE(sphere1.getCentre() == glm::vec3(0.0f));

    IntersectData result = sphere1.intersectBoundingSphere(sphere2);
    REQUIRE(result.isIntersecting());

    // Using Collider class
    REQUIRE(sphere1.intersect(sphere2).isIntersecting());

    // Unsupported collider test
    FakeCollider fakeCollider;
    REQUIRE_FALSE(sphere1.intersect(fakeCollider).isIntersecting());
}

TEST_CASE("BoundingSphere intersecting AABB", "[BoundingSphere]") {
    BoundingSphere sphere(glm::vec3(0.0f), 1.0f);
    BoundingAABB aabb(glm::vec3(0.0f), glm::vec3(1.0f));

    IntersectData result = sphere.intersectAABB(aabb);
    REQUIRE(result.isIntersecting());

    // Using Collider class
    REQUIRE(sphere.intersect(aabb).isIntersecting());
}

TEST_CASE("AABB intersecting AABB", "[BoundingAABB]") {
    BoundingAABB aabb1(glm::vec3(0.0f), glm::vec3(1.0f));
    BoundingAABB aabb2(glm::vec3(0.5f), glm::vec3(1.0f));

    REQUIRE(aabb1.getCentre() == glm::vec3(0.5f));
    REQUIRE(aabb1.getMinExtents() == glm::vec3(0.0f));
    REQUIRE(aabb1.getMaxExtents() == glm::vec3(1.0f));

    IntersectData result = aabb1.intersectAABB(aabb2);
    REQUIRE(result.isIntersecting());

    // Using Collider class
    REQUIRE(aabb1.intersect(aabb2).isIntersecting());
}


TEST_CASE("BoundingSphere transform", "[BoundingSphere]") {
    BoundingSphere sphere(glm::vec3(0.0f), 1.0f);

    sphere.transform(glm::vec3(1.0f));
    REQUIRE(sphere.getCentre() == glm::vec3(1.0f));
}

TEST_CASE("AABB transform", "[BoundingSphere]") {
    BoundingAABB aabb(glm::vec3(0.0f), glm::vec3(1.0f));

    aabb.transform(glm::vec3(1.0f));
    REQUIRE(aabb.getMinExtents() == glm::vec3(1.0f));
}

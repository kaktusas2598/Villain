#include "catch2/catch_approx.hpp"
#include "catch2/catch_test_macros.hpp"

#include "glm/gtx/string_cast.hpp"
#include "physics/Particle.hpp"
#include <glm/gtc/epsilon.hpp>

TEST_CASE("Particle integration", "[Particle]") {
    SECTION("Infinite mass particle integration") {
        Villain::Particle particle;
        particle.setInverseMass(0.0f);
        particle.setPosition(glm::vec3(1.0f, 2.0f, 3.0f));
        particle.setVelocity(glm::vec3(2.0f, 0.0f, 1.0f));
        particle.setAcceleration(glm::vec3(0.0f, 1.0f, 0.0f));
        particle.setDamping(0.9f);

        particle.integrate(0.1f);

        // Expect no change in position, velocity, and acceleration
        REQUIRE(particle.getPosition() == glm::vec3(1.0f, 2.0f, 3.0f));
        REQUIRE(particle.getVelocity() == glm::vec3(2.0f, 0.0f, 1.0f));
        REQUIRE(particle.getAcceleration() == glm::vec3(0.0f, 1.0f, 0.0f));
    }

    SECTION("Finite mass particle integration") {
        Villain::Particle particle;
        particle.setInverseMass(0.5f);
        particle.setPosition(glm::vec3(1.0f, 2.0f, 3.0f));
        particle.setVelocity(glm::vec3(2.0f, 0.0f, 1.0f));
        particle.setAcceleration(glm::vec3(0.0f, 1.0f, 0.0f));
        particle.setDamping(0.9f);

        particle.integrate(0.1f);

        // Expect updated position, velocity, and acceleration based on integration formulas
        REQUIRE(particle.getPosition() == glm::vec3(1.2f, 2.0f, 3.1f));
        REQUIRE(glm::all(glm::epsilonEqual(particle.getVelocity(), glm::vec3(1.979038, 0.098952, 0.989519), 0.001f)));
    }
}

TEST_CASE("Particle lifetime", "[Particle]") {
    SECTION("Particle with indefinite lifespan") {
        Villain::Particle particle;
        particle.setPosition(glm::vec3(0.0f));
        particle.setVelocity(glm::vec3(1.0f));
        particle.setAcceleration(glm::vec3(0.0f));
        particle.setInverseMass(1.0f);
        particle.setDamping(0.9f);

        particle.integrate(0.1f);

        // Expect the particle to still be alive after integration
        REQUIRE(particle.isAlive());
    }

    SECTION("Particle with finite lifespan") {
        Villain::Particle particle;
        particle.setPosition(glm::vec3(0.0f));
        particle.setVelocity(glm::vec3(1.0f));
        particle.setAcceleration(glm::vec3(0.0f));
        particle.setInverseMass(1.0f);
        particle.setDamping(0.9f);
        particle.setLifetime(0.5f);

        particle.integrate(0.1f);
        REQUIRE(particle.isAlive());

        particle.integrate(0.4f);
        REQUIRE(!particle.isAlive());
    }
}


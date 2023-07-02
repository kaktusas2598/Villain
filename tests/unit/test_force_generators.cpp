#include "catch2/catch_approx.hpp"
#include "catch2/catch_test_macros.hpp"

#include "physics/ParticleForceGenerator.hpp"
#include "physics/generators/ParticleGravity.hpp"
#include <glm/gtc/epsilon.hpp>

TEST_CASE("ParticleForceGenerator - Gravity", "[ParticleForceGenerator]") {
    SECTION("ApplyForce") {
        Villain::Particle particle;
        particle.setInverseMass(2.0f);
        particle.setPosition(glm::vec3(1.0f, 2.0f, 3.0f));
        particle.setAcceleration(glm::vec3(0.0f));
        particle.setDamping(0.9f);

        Villain::ParticleGravity gravity(glm::vec3(0.0f, -9.8f, 0.0f));
        gravity.updateForce(&particle, 0.1f);

        // Expect updated force accumulator based on gravity force
        REQUIRE(particle.getForce() == glm::vec3(0.0f, -4.9f, 0.0f));
    }
}

TEST_CASE("ParticleForceRegistry", "[ParticleForceRegistry]") {
    SECTION("Add and Remove Force Generator") {
        Villain::Particle particle;
        particle.setInverseMass(1.0f);
        particle.setPosition(glm::vec3(1.0f, 2.0f, 3.0f));
        particle.setVelocity(glm::vec3(2.0f, 0.0f, 1.0f));
        particle.setAcceleration(glm::vec3(0.0f));
        particle.setDamping(0.9f);

        Villain::ParticleGravity gravity(glm::vec3(0.0f, -9.8f, 0.0f));
        Villain::ParticleForceRegistry registry;

        registry.add(&particle, &gravity);

        registry.updateForces(0.1f);

        // Expect updated force accumulator based on gravity force
        REQUIRE(particle.getForce() == glm::vec3(0.0f, -9.8f, 0.0f));

        registry.remove(&particle, &gravity);

        registry.updateForces(0.1f);

        // No change from previous update as generator was removed, but particle wasn't integrated yet
        REQUIRE(particle.getForce() == glm::vec3(0.0f, -9.8f, 0.0f));


        // Testing clear() method
        registry.add(&particle, &gravity);
        registry.clear();
        registry.updateForces(0.1f);

        // No change from previous update as generator was removed, but particle wasn't integrated yet
        REQUIRE(particle.getForce() == glm::vec3(0.0f, -9.8f, 0.0f));

    }
}

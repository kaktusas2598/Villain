#include "catch2/catch_approx.hpp"
#include "catch2/catch_test_macros.hpp"
#include <glm/gtc/epsilon.hpp>
#include <glm/gtx/string_cast.hpp>

#include "physics/generators/ParticleAnchoredBungee.hpp"
#include "physics/generators/ParticleAnchoredSpring.hpp"
#include "physics/generators/ParticleBungee.hpp"
#include "physics/generators/ParticleBuoyancy.hpp"
#include "physics/generators/ParticleDrag.hpp"
#include "physics/generators/ParticleFakeSpring.hpp"
#include "physics/generators/ParticleGravity.hpp"
#include "physics/generators/ParticleSpring.hpp"

TEST_CASE("ParticleForceGenerator - Gravity", "[ParticleForceGenerator]") {
    SECTION("Update Force") {
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

TEST_CASE("ParticleForceGenerator - Drag", "[ParticleForceGenerator]") {
    SECTION("Update Force") {
        Villain::Particle particle;
        particle.setInverseMass(2.0f);
        particle.setPosition(glm::vec3(1.0f, 2.0f, 3.0f));
        particle.setVelocity(glm::vec3(2.0f, 1.0f, 3.0f));
        particle.setAcceleration(glm::vec3(0.0f));
        particle.setDamping(0.9f);

        Villain::ParticleDrag drag(0.5f, 0.2f);
        drag.updateForce(&particle, 0.1f);

        // Expect updated force accumulator based on drag force
        REQUIRE(glm::all(glm::epsilonEqual(particle.getForce(), glm::vec3(-2.496663, -1.248332, -3.744994), 0.01f)));
    }
}

TEST_CASE("ParticleForceGenerator - Spring", "[ParticleForceGenerator]") {
    SECTION("Update Force") {
        Villain::Particle particle;
        particle.setInverseMass(1.0f);
        particle.setPosition(glm::vec3(1.0f, 2.0f, 3.0f));
        particle.setAcceleration(glm::vec3(0.0f));
        particle.setDamping(0.9f);

        Villain::Particle otherParticle;
        otherParticle.setInverseMass(1.0f);
        otherParticle.setPosition(glm::vec3(2.0f, 3.0f, 4.0f));
        otherParticle.setAcceleration(glm::vec3(0.0f));
        otherParticle.setDamping(0.9f);

        Villain::ParticleSpring spring(&otherParticle, 2.0f, 1.5f);
        spring.updateForce(&particle, 0.1f);

        // Expect updated force accumulator based on spring force
        REQUIRE(glm::all(glm::epsilonEqual(particle.getForce(), glm::vec3(0.267949, 0.267949, 0.267949), 0.01f)));
    }
}

TEST_CASE("ParticleForceGenerator - AnchoredSpring", "[ParticleForceGenerator]") {
    SECTION("Update Force") {
        Villain::Particle particle;
        particle.setInverseMass(1.0f);
        particle.setPosition(glm::vec3(1.0f, 2.0f, 3.0f));
        particle.setAcceleration(glm::vec3(0.0f));
        particle.setDamping(0.9f);

        Villain::ParticleAnchoredSpring anchoredSpring({0.0, 10.0f, 1.0f}, 2.0f, 1.5f);
        anchoredSpring.updateForce(&particle, 0.1f);

        // Expect updated force accumulator based on spring force
        REQUIRE(glm::all(glm::epsilonEqual(particle.getForce(), glm::vec3(-1.638842, 13.110739, -3.277685), 0.01f)));
    }
}

TEST_CASE("ParticleForceGenerator - Bungee", "[ParticleForceGenerator]") {
    SECTION("Update Force") {
        Villain::Particle particle;
        particle.setInverseMass(1.0f);
        particle.setPosition(glm::vec3(1.0f, 2.0f, 3.0f));
        particle.setAcceleration(glm::vec3(0.0f));
        particle.setDamping(0.9f);

        Villain::Particle otherParticle;
        otherParticle.setInverseMass(1.0f);
        otherParticle.setPosition(glm::vec3(2.0f, 3.0f, 4.0f));
        otherParticle.setAcceleration(glm::vec3(0.0f));
        otherParticle.setDamping(0.9f);

        Villain::ParticleBungee bungee(&otherParticle, 2.0f, 1.5f);
        bungee.updateForce(&particle, 0.1f);

        // Expect updated force accumulator based on spring force
        REQUIRE(glm::all(glm::epsilonEqual(particle.getForce(), glm::vec3(0.267949, 0.267949, 0.267949), 0.01f)));
    }
}

TEST_CASE("ParticleForceGenerator - AnchoredBungee", "[ParticleForceGenerator]") {
    SECTION("Update Force") {
        Villain::Particle particle;
        particle.setInverseMass(1.0f);
        particle.setPosition(glm::vec3(1.0f, 2.0f, 3.0f));
        particle.setAcceleration(glm::vec3(0.0f));
        particle.setDamping(0.9f);

        Villain::ParticleAnchoredBungee anchoredBungee({0.0, 10.0f, 1.0f}, 2.0f, 1.5f);
        anchoredBungee.updateForce(&particle, 0.1f);

        // Expect updated force accumulator based on spring force
        REQUIRE(glm::all(glm::epsilonEqual(particle.getForce(), glm::vec3(-1.638842, 13.110739, -3.277685), 0.01f)));
    }
}

// NOTE: not sure if this is really a good test case
TEST_CASE("ParticleForceGenerator - Buoyancy", "[ParticleForceGenerator]") {
    SECTION("Update Force") {
        Villain::Particle particle;
        particle.setInverseMass(1.0f);
        particle.setPosition(glm::vec3(1.0f, 2.0f, 3.0f));
        particle.setAcceleration(glm::vec3(0.0f));
        particle.setDamping(0.9f);

        Villain::ParticleBuoyancy buoyancy(0.4f, 0.01f, 5.0f);
        buoyancy.updateForce(&particle, 0.1f);

        // Expect updated force accumulator based on spring force
        REQUIRE(glm::all(glm::epsilonEqual(particle.getForce(), glm::vec3(0.0, 10.0, 0.0), 0.01f)));
    }

}

TEST_CASE("ParticleForceGenerator - FakeSpring", "[ParticleForceGenerator]") {
    SECTION("Update Force") {
        Villain::Particle particle;
        particle.setInverseMass(1.0f);
        particle.setPosition(glm::vec3(0.0f, 5.0f, 0.0f));
        particle.setAcceleration(glm::vec3(0.0f));
        particle.setDamping(0.9f);

        Villain::ParticleFakeSpring stiffSpring({0.0, 10.0, 0.0}, 1000.0f, 0.1f);
        stiffSpring.updateForce(&particle, 0.1f);

        // Expect updated force accumulator based on spring force
        REQUIRE(glm::all(glm::epsilonEqual(particle.getForce(), glm::vec3(0.000000, 9.974161, 0.000000), 0.01f)));
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

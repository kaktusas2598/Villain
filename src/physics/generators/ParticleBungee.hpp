#pragma once

#include "physics/ParticleForceGenerator.hpp"

namespace Villain {

    // Force generator which applies spring force to 2 particles, but spring can only extend
    class ParticleBungee: public ParticleForceGenerator {
        public:
            ParticleBungee(Particle* otherParticle, float stifnessConstant, float springRestLength)
                : other(otherParticle), springConstant(stifnessConstant), restLength(springRestLength) {}

            virtual void updateForce(Particle* particle, float deltaTime) {
                // Calculate vector of the spring
                glm::vec3 force = particle->getPosition();
                force -= other->getPosition();

                float magnitude = glm::length(force);
                // Check if the bungee is compressed
                if (magnitude <= restLength) return;

                // Calculate magnitude of the force
                magnitude = springConstant * (restLength - magnitude);

                // Calculate final force and apply it
                force = glm::normalize(force);
                force *= -magnitude;
                particle->addForce(force);
            }

        private:
            Particle* other; //< Particle at the other end of the spring
            float springConstant; //< Spring constant as seen in Hook's law
            float restLength; //< Spring's rest length with no forces acting upon it
    };
}

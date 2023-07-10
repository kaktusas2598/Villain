#pragma once

#include "physics/ParticleForceGenerator.hpp"

namespace Villain {

    // Force generator which applies spring force to 2 particles
    class ParticleSpring: public ParticleForceGenerator {
        public:
            ParticleSpring(Particle* otherParticle, float stifnessConstant, float springRestLength)
                : other(otherParticle), springConstant(stifnessConstant), restLength(springRestLength) {}

            virtual void updateForce(Particle* particle, float deltaTime) {
                // Calculate vector of the spring
                glm::vec3 force = particle->getPosition();
                force -= other->getPosition();

                // Calculate magnitude of the force
                float magnitude = glm::length(force);
                magnitude = fabsf(magnitude - restLength);
                magnitude *= springConstant;

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

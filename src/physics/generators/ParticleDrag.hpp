#pragma once

#include "physics/ParticleForceGenerator.hpp"

namespace Villain {

    class ParticleDrag: public ParticleForceGenerator {
        public:
            ParticleDrag(float c1, float c2) : k1(c1), k2(c2) {}

            virtual void updateForce(Particle* particle, float deltaTime) {
                // Calculate the magnitude of the velocity
                float velocityMag = glm::length(particle->getVelocity());

                // Calculate total drag coefficient
                float dragCoeff = velocityMag;
                dragCoeff = k1 * dragCoeff + k2 * dragCoeff * dragCoeff;

                // Calculate final force and apply it
                glm::vec3 force = glm::normalize(particle->getVelocity());
                force *= -dragCoeff;
                particle->addForce(force);
            }

        private:
            float k1; //< Velocity drag coefficient
            float k2; //< Velocity squared drag coefficient
    };
}

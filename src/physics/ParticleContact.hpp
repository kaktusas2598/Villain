#pragma once

#include "Particle.hpp"

namespace Villain {

    class ParticleContactResolver;

    // Represents the details of particle contact
    class ParticleContact {
        // Resolver needs to be able to access contact details to resolve them
        friend class ParticleContactResolver;

        public:
            Particle* particle[2]; //< Particles involved in contact, 2nd one can be null
            float restitution; //< Normal restitution coefficient at the contact, depends on materials
            glm::vec3 contactNormal; //< Direction of contact (from the perspective of the first object)
            float penetration; //< Penetration depth
            glm::vec3 particleMovement[2]{}; //< Amount each particle is moved during the interpenetration

        protected:
            void resolve(float deltaTime);
            float calculateSeparatingVelocity() const;

        private:
            // Handles impulse calculations for collision resolution
            void resolveVelocity(float deltaTime);
            // Handles the interpenetration resolution for this contact
            void resolveInterpenetration(float deltaTime);
    };

    // Contact resolution routine for particle contacts. One instance can be used
    // throughout the whole simulation
    class ParticleContactResolver {
        public:
            ParticleContactResolver(unsigned iterations);

            void setIterations(unsigned iterations) { numIterations = iterations; }
            // Number of iterations should be at least the number of contacts.
            // Delta time here is duration of previous integration step to compensate for the forces applied
            void resolveContacts(ParticleContact* contactArray, unsigned numContacts, float deltaTime);
        protected:
            unsigned numIterations; //< Number of iterations allowed
            unsigned iterationsUsed; //< Performance tracking value
    };

    // Interface for contact generators which apply to particles
    class ParticleContactGenerator {
        public:
            // Fills givent contact structure with generated contact. Contact pointer should point to
            // the first available contact in contact array, where limit is the maximum number of contacts that
            // can be written to. Returns number of contacts written to
            virtual unsigned addContact(ParticleContact* contact, unsigned limit) = 0;
    };
}

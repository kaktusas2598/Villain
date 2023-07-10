#pragma once

#include "physics/ParticleContact.hpp"

namespace Villain {

    // Like particle link but instead of linking 2 particles, links 1 particle to anchored point in space
    class ParticleConstraint : public ParticleContactGenerator {
        public:
            ParticleConstraint(Particle* p, const glm::vec3& anchorPoint) : particle(p), anchor(anchorPoint) {}

            // Replicated from base class here for documentation purposes
            // Generates a contact to keep this link from being violated, can only be used to generate a single
            // contact, where limit is expected to be 1 and 0 is invalid value. Will return 1 if contact is needed and
            // 0 if link wasn't violated
            virtual unsigned addContact(ParticleContact* contact, unsigned limit) const = 0;

        protected:
            float getCurrentLenght() const {
                glm::vec3 relativePos = particle->getPosition() - anchor;
                return glm::length(relativePos);
            }

            glm::vec3 anchor;
            Particle* particle;
    };
}

#pragma once

#include "physics/ParticleContact.hpp"

namespace Villain {

    // Links two particles together, generating a contact if they violate the constraints of the link
    // Used as a based class for cables and rods and can be used as a base for springs with a limit to their extension
    class ParticleLink : public ParticleContactGenerator {
        public:
            ParticleLink(Particle* p0, Particle* p1) {
                particle[0] = p0;
                particle[1] = p1;
            }
            // Replicated from base class here for documentation purposes
            // Generates a contact to keep this link from being violated, can only be used to generate a single
            // contact, where limit is expected to be 1 and 0 is invalid value. Will return 1 if contact is needed and
            // 0 if link wasn't violated
            virtual unsigned addContact(ParticleContact* contact, unsigned limit) const = 0;

        protected:
            float getCurrentLenght() const {
                glm::vec3 relativePos = particle[0]->getPosition() - particle[1]->getPosition();
                return glm::length(relativePos);
            }
            Particle* particle[2]; //< Particles connected by this link
    };
}

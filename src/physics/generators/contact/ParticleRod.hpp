#pragma once

#include "ParticleLink.hpp"

namespace Villain {

    // Rod links to particles in a pair keeping them at fixed distance from each other and
    // generating max 2 contacts if they stray too far apart or are too close
    class ParticleRod : public ParticleLink {
        public:
            // Fill in given contact pointer with contact needed to prevent the rod from overextending or compressing
            virtual unsigned addContact(ParticleContact* contact, unsigned limit) const override {
                float currentLength = getCurrentLenght();

                // Check if rod is overextended
                if (currentLength == lenght) return 0;

                // Otherwise, get the contact
                contact->particle[0] = particle[0];
                contact->particle[1] = particle[1];

                // Calculate the normal
                glm::vec3 normal = glm::normalize(particle[1]->getPosition() - particle[0]->getPosition());

                // Normal depends on whether rod is overextending or compressing
                if (currentLength > lenght) {
                    contact->contactNormal = normal;
                    contact->penetration = currentLength - lenght;
                } else {
                    contact->contactNormal = normal * -1.0f;
                    contact->penetration = lenght - currentLength;
                }

                // No bounciness for rods so restituion is always zero!
                contact->restitution = 0.0f;

                return 1;

            }

        private:
            float lenght; //< lenght of this rod
    };
}

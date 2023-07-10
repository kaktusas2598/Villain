#pragma once

#include "ParticleConstraint.hpp"

namespace Villain {

    // Links particle to anchor in space and generates a contact if they compress or overextend
    class ParticleRodConstraint : public ParticleConstraint {
        public:
            ParticleRodConstraint(float rodLenght, Particle* p, const glm::vec3& anchorPoint)
                : ParticleConstraint(p, anchorPoint), lenght(rodLenght) {}

            // Fill in given contact pointer with contact needed to prevent the rod from overextending or compressing
            virtual unsigned addContact(ParticleContact* contact, unsigned limit) const override {
                float currentLength = getCurrentLenght();

                // Check if rod is overextended
                if (currentLength == lenght) return 0;

                // Otherwise, get the contact
                contact->particle[0] = particle;
                contact->particle[1] = 0;

                // Calculate the normal
                glm::vec3 normal = glm::normalize(anchor - particle->getPosition());

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

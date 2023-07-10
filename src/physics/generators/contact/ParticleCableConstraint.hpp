#pragma once

#include "ParticleConstraint.hpp"

namespace Villain {

    // Links particle to an anchord point generating a contact if they stray too far apart
    class ParticleCableConstraint : public ParticleConstraint {
        public:
            ParticleCableConstraint(float maxCableLength, float rest, Particle* p, const glm::vec3& anchorPoint)
                : ParticleConstraint(p, anchorPoint), maxLenght(maxCableLength), restitution(rest) {}

            // Fill in given contact pointer with contact needed to prevent the cable from overextending
            virtual unsigned addContact(ParticleContact* contact, unsigned limit) const override {
                float lenght = getCurrentLenght();

                // Check if the cable is overextended
                if (lenght < maxLenght) return 0;

                // Otherwise, get the contact
                contact->particle[0] = particle;
                contact->particle[1] = nullptr;

                // Calculate normal to pull objects together
                glm::vec3 normal = glm::normalize(anchor - particle->getPosition());
                contact->contactNormal = normal;

                contact->penetration = lenght - maxLenght;
                contact->restitution = restitution;

                return 1;
            }

        private:
            float maxLenght; //< Maximum lenght of this cable
            float restitution; //< Coefficient of restitution(bounciness) of the cable
    };
}

#pragma once

#include "ParticleLink.hpp"

namespace Villain {

    // Cables link a pair of particles generating a contact if they stray too far apart
    class ParticleCable : public ParticleLink {
        public:
            // Fill in given contact pointer with contact needed to prevent the cable from overextending
            virtual unsigned addContact(ParticleContact* contact, unsigned limit) const override {
                float lenght = currentLenght();

                // Check if the cable is overextended
                if (lenght < maxLenght) {
                    return 0;
                }

                // Otherwise, get the contact
                contact->particle[0] = particle[0];
                contact->particle[1] = particle[1];

                // Calculate normal to pull objects together
                glm::vec3 normal = glm::normalize(particle[1]->getPosition() - particle[0]->getPosition());
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

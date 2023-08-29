#pragma once

#include "physics/ParticleContact.hpp"
#include <vector>

namespace Villain {

    // Generates contacts for a vector of particle to cause them to collide against the ground
    class GroundContacts : public ParticleContactGenerator {
        public:
            GroundContacts(std::vector<Particle*>& particles, float groundHeight = 0.0f)
                : particles(particles), height(groundHeight) {}

            virtual unsigned addContact(ParticleContact* contact, unsigned limit) const override {
                unsigned count = 0;

                for (Particle* p : particles) {
                    float y = p->getPosition().y;

                    if (y < height) {
                        contact->contactNormal = glm::vec3(0.0, 1.0, 0.0); // Up vector
                        contact->particle[0] = p;
                        contact->particle[1] = nullptr;
                        contact->penetration = height - y;
                        contact->restitution = 0.2f;
                        contact++;
                        count++;
                    }

                    if (count >= limit) return count;
                }

                return count;
            }
        private:
            float height; //< Default height on y axis
            std::vector<Particle*> particles;

    };
}

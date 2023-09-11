#pragma once

#include "physics/Contact.hpp"
#include "terrain/Terrain.hpp"

namespace Villain {

    // Generates contacts for a rigid body to cause them to collide against the terrain
    class TerrainContacts : public ContactGenerator {
        public:
            TerrainContacts(RigidBody* body, Terrain* terrain)
                : body(body), terrain(terrain) {}

            // TODO: This is too basic, only providing one contact, also need to take in account terrain's
            // and possibly rigid body's geometry and possible produce multiple contact points!
            virtual unsigned addContact(Contact* contact, unsigned limit) const override {
                unsigned count = 0;

                // Get the current position of the rigid body
                glm::vec3 bodyPosition = body->getPosition();

                // Calculate the height of the terrain at the body's x and z position
                float terrainHeight = terrain->getHeight(bodyPosition.x, bodyPosition.z);

                // Calculate the penetration depth (how much the body is below the terrain)
                float penetration = bodyPosition.y - terrainHeight;

                // Check if the body has penetrated the terrain
                if (penetration < 0) {
                    contact->contactNormal = glm::vec3(0.0, 1.0, 0.0); // Up vector
                    contact->contactPoint = bodyPosition - glm::vec3(0.0, penetration, 0.0);
                    contact->penetration = -penetration; // Make it positive
                    contact->restitution = 0.2f; // TODO:stop hardcoding restitution and friction everywhere!
                    contact->setBodyData(body, nullptr, 1.0f, 0.0f);
                    contact++;
                    count++;
                }

            return count;

                return count;
            }
        private:
            RigidBody* body;
            Terrain* terrain;

    };
}

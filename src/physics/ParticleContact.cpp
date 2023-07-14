#include "ParticleContact.hpp"

namespace Villain {

    void ParticleContact::resolve(float deltaTime) {
        // Possible to investigate time-division engine approach which is more time consuming but requires
        // no resolving intepenetration, that way we have multiple updates per frame handling collisions
        resolveVelocity(deltaTime);
        resolveInterpenetration(deltaTime);
    }

    float ParticleContact::calculateSeparatingVelocity() const {
        glm::vec3 relativeVelocity = particle[0]->getVelocity();
        if (particle[1]) relativeVelocity -= particle[1]->getVelocity();
        return glm::dot(relativeVelocity, contactNormal);
    }

    void ParticleContact::resolveVelocity(float deltaTime) {
        // find the separating velocity in the direction of the contact
        float separatingVelocity = calculateSeparatingVelocity();

        if (separatingVelocity > 0) {
            // The contact is either separating or stationany - no need to
            // calculate the impule
            return;
        }

        float newSeparatingVelocity = -separatingVelocity * restitution;

        // Check the velocity buildup due to acceleration only
        // This is to prevent resting contacts from vibrating and calculate enough velocity to prevent that each frame
        // This is called microcollision approach
        // This is somewhat a hacky approach to manage resting contacts and more physically accurate simulation would produce
        // reaction force from the ground for example
        glm::vec3 accCausedVelocity = particle[0]->getAcceleration();
        if (particle[1]) accCausedVelocity -= particle[1]->getAcceleration();
        float accCausedSepVelocity = glm::dot(accCausedVelocity, contactNormal) * deltaTime;
        // If there is closing velocity due to acceleration buildup, remobe it from the new separating velicity
        if (accCausedSepVelocity < 0) {
            newSeparatingVelocity += restitution * accCausedSepVelocity;
            // Make sure not to remove too much
            if (newSeparatingVelocity < 0) newSeparatingVelocity = 0;
        }

        float deltaVelocity = newSeparatingVelocity - separatingVelocity;

        float totalInverseMass = particle[0]->getInverseMass();
        if (particle[1]) totalInverseMass += particle[1]->getInverseMass();

        // If both particles have infinite mass then no impulse is applied
        if (totalInverseMass <= 0) return;

        float impulse = deltaVelocity / totalInverseMass;
        glm::vec3 impulsePerIMass = contactNormal * impulse;

        // Apply impulses in direction of the contact, impulses are proportional
        // to inverse mass
        particle[0]->setVelocity(particle[0]->getVelocity() + impulsePerIMass * particle[0]->getInverseMass());
        // If 2nd particle exists (not an immovable object) then it will go in opposite direction
        if (particle[1]) {
            particle[1]->setVelocity(particle[1]->getVelocity() + impulsePerIMass * -particle[1]->getInverseMass());
        }
    }

    void ParticleContact::resolveInterpenetration(float deltaTime) {
        // Objects are barely touching or separated so no action needs to be taken
        if (penetration <= 0) return;

        // Movement of each object is based on their inverse mass
        float totalInverseMass = particle[0]->getInverseMass();
        if (particle[1]) totalInverseMass += particle[1]->getInverseMass();

        // If both particles have infinite mass then no action needs to be taken
        if (totalInverseMass <= 0) return;

        // Find amount of penetration resolution per unit of inverse mass
        glm::vec3 movePerIMass = contactNormal * (penetration / totalInverseMass);

        // Calculate movement amounts to separate objects
        particleMovement[0] = movePerIMass * particle[0]->getInverseMass();
        if (particle[1]) {
            particleMovement[1] = movePerIMass * -particle[1]->getInverseMass();
        } else {
            particleMovement[1] = glm::vec3(0.0, 0.0, 0.0);
        }

        // Apply the penetration resolution
        particle[0]->setPosition(particle[0]->getPosition() + particleMovement[0]);
        if (particle[1]) {
            particle[1]->setPosition(particle[1]->getPosition() + particleMovement[1]);
        }
    }

    ParticleContactResolver::ParticleContactResolver(unsigned iterations): numIterations(iterations) {}

    void ParticleContactResolver::resolveContacts(ParticleContact* contactArray, unsigned numContacts, float deltaTime) {
        unsigned i;
        iterationsUsed = 0;

        while (iterationsUsed < numIterations) {
            // Find the contact with largest closing velocity
            float max = FLT_MAX;
            unsigned maxIndex = numContacts;
            for (i = 0; i < numContacts; i++) {
                float sepVel = contactArray[i].calculateSeparatingVelocity();
                if (sepVel < max && sepVel < 0 || contactArray[i].penetration > 0) {
                    max = sepVel;
                    maxIndex = i;
                }
            }

            // Is there anything worth resolving?
            if (maxIndex == numContacts) break;

            // Resolve this contact
            contactArray[maxIndex].resolve(deltaTime);

            //////////////////////
            // Update interpenetration for all particles
            glm::vec3* move = contactArray[maxIndex].particleMovement;
            for (i = 0; i < numContacts; i++) {
                if (contactArray[i].particle[0] == contactArray[maxIndex].particle[0]) {
                    contactArray[i].penetration -= glm::dot(move[0], contactArray[i].contactNormal);
                } else if (contactArray[i].particle[0] == contactArray[maxIndex].particle[1]) {
                    contactArray[i].penetration -= glm::dot(move[1], contactArray[i].contactNormal);
                }
                if (contactArray[i].particle[1]) {
                    if (contactArray[i].particle[1] == contactArray[maxIndex].particle[0]) {
                        contactArray[i].penetration += glm::dot(move[0], contactArray[i].contactNormal);
                    } else if (contactArray[i].particle[1] == contactArray[maxIndex].particle[1]) {
                        contactArray[i].penetration += glm::dot(move[1], contactArray[i].contactNormal);
                    }
                }
            }
            //////////////////////

            iterationsUsed++;
        }
    }
}

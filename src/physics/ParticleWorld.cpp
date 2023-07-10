#include "ParticleWorld.hpp"

namespace Villain {

    ParticleWorld::ParticleWorld(unsigned maxContacts, unsigned iterations)
        : numMaxContacts(maxContacts), resolver(iterations) {
            contacts = new ParticleContact[numMaxContacts];
            calculateIterations = (iterations == 0);
    }

    ParticleWorld::~ParticleWorld() {
        delete[] contacts;
    }

    void ParticleWorld::startFrame() {
        for (Particles::iterator p = particles.begin(); p != particles.end(); p++) {
            (*p)->clearAccumulator();
        }
    }

    unsigned ParticleWorld::generateContacts() {
        unsigned limit = numMaxContacts;
        ParticleContact* nextContact = contacts;

        for (ContactGenerators::iterator g = contactGenerators.begin(); g != contactGenerators.end() ; g++) {
            unsigned used = (*g)->addContact(nextContact, limit);
            limit -= used;
            nextContact += used;

            // Ran out of contacts to fill, so we are missing contacts
            if (limit <= 0) break;
        }

        // Return number of contacts used;
        return numMaxContacts - limit;
    }

    void ParticleWorld::integrate(float deltaTime) {
        for (Particles::iterator p = particles.begin(); p != particles.end(); p++) {
            (*p)->integrate(deltaTime);
        }
    }

    void ParticleWorld::runPhysics(float deltaTime) {
        // First apply all the force generators
        registry.updateForces(deltaTime);

        // Then integrate particles
        integrate(deltaTime);

        // Gemerate contacts
        unsigned usedContacts = generateContacts();

        // And finally resolve contacts
        if (usedContacts) {
            if (calculateIterations) resolver.setIterations(usedContacts * 2);
            resolver.resolveContacts(contacts, usedContacts, deltaTime);
        }
    }
}

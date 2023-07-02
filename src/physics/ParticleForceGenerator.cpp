#include "ParticleForceGenerator.hpp"

#include <algorithm>

namespace Villain {

    void ParticleForceRegistry::add(Particle* particle, ParticleForceGenerator* generator) {
        registry.push_back({particle, generator});
    }

    void ParticleForceRegistry::remove(Particle* particle, ParticleForceGenerator* generator) {
        registry.erase(
            std::remove_if(registry.begin(), registry.end(),
                [particle, generator](const ParticleForceRegistration& registration) {
                    return registration.particle == particle && registration.generator == generator;
                }
            ), registry.end());
    }

    void ParticleForceRegistry::clear() {
        registry.clear();
    }

    void ParticleForceRegistry::updateForces(float deltaTime) {
        for (auto& registration: registry) {
            registration.generator->updateForce(registration.particle, deltaTime);
        }
    }
}


#include "ForceGenerator.hpp"

#include <algorithm>

namespace Villain {

    void ForceRegistry::add(RigidBody* body, ForceGenerator* generator) {
        registry.push_back({body, generator});
    }

    void ForceRegistry::remove(RigidBody* body, ForceGenerator* generator) {
        registry.erase(
            std::remove_if(registry.begin(), registry.end(),
                [body, generator](const ForceRegistration& registration) {
                    return registration.body == body && registration.generator == generator;
                }
            ), registry.end());
    }

    void ForceRegistry::clear() {
        registry.clear();
    }

    void ForceRegistry::updateForces(float deltaTime) {
        for (auto& registration: registry) {
            registration.generator->updateForce(registration.body, deltaTime);
        }
    }


}

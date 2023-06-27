#include "Particle.hpp"

namespace Villain {

    void Particle::integrate(float duration) {
        if (inverseMass <= 0.0f) return;  // Infinite mass objects don't move

        // Update linear position, velocity, and acceleration Newton-Euler using numerical integration
        position += velocity * duration;
        // TODO: implement general forces
        glm::vec3 resultingAcc = acceleration * (1.0f / inverseMass);
        velocity += resultingAcc * duration;
        velocity *= powf(damping, duration);  // Impose drag

        // Reset acceleration to zero for the next integration step
        clearAccumulator();
    }
}
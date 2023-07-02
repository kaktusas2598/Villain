#include "Particle.hpp"

namespace Villain {

    void Particle::integrate(float duration) {
        if (lifetime >= 0.0f) {
            age += duration;
        }

        if (inverseMass <= 0.0f) return;  // Infinite mass objects don't move

        // Update linear position, velocity, and acceleration Newton-Euler using numerical integration
        position += velocity * duration;

        // D'Alembert's principle - resulting force is sum of all forces affecting particle
        glm::vec3 resultingAcc = acceleration;
        resultingAcc += forceAccum * inverseMass;

        velocity += resultingAcc * duration;
        velocity *= powf(damping, duration);  // Impose drag

        // Reset acceleration and force accumulator to zero for the next integration step
        clearAccumulator();
    }
}

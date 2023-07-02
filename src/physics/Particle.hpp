#pragma once

#include <glm/glm.hpp>

namespace Villain {

    // Simplest physical body, that can be simulated, point mass object
    class Particle {
        public:
            Particle() :
                position(0.0f),
                velocity(0.0f),
                acceleration(0.0f),
                inverseMass(1.0f), // Assuming unit mass by default
                damping(0.98f), // A reasonable damping value between 0 and 1
                forceAccum(0.0f)
            {}

            void integrate(float duration);

            // Add force to particle applied only during the next integration step
            void addForce(const glm::vec3& force) {
                forceAccum += force;
            }

            glm::vec3 getForce() const { return forceAccum; }
            glm::vec3 getPosition() const { return position; }
            glm::vec3 getVelocity() const { return velocity; }
            glm::vec3 getAcceleration() const { return acceleration; }
            float getInverseMass() const { return inverseMass; }
            float getDamping() const { return damping; }
            float getLifetime() const { return lifetime; }
            float getAge() const { return age; }

            void setPosition(const glm::vec3& pos) { position = pos; }
            void setVelocity(const glm::vec3& vel) { velocity = vel; }
            void setAcceleration(const glm::vec3& acc) { acceleration = acc; }
            void setMass(float m) { inverseMass = 1/m; }
            void setInverseMass(float m) { inverseMass = m; }
            void setDamping(float d) { damping = d; }
            void setLifetime(float l) { lifetime = l; }

            bool isAlive() {
                if (lifetime <= 0.0f) return true;
                return age < lifetime;
            }

        protected:
            glm::vec3 position; //< Linear position in world space
            glm::vec3 velocity; //< Linear velocity in world space
            glm::vec3 acceleration; //< Linear acceleration in world space
            float inverseMass; //< Inverse mass is more useful in numerical integration and for simulating infinite mass bodies

            float damping; //< Damping applied to linear motion to simulate real world drag

            glm::vec3 forceAccum; //< Accumulated force applied at next integration only and cleared after

            // NOTE: Not sure if we want to keep these here, or possibly inherit Particle
            float lifetime = -1; //<<< By default particle will live forever;
            float age = 0;

        private:
            void clearAccumulator() {
                acceleration = glm::vec3(0.0f);
                forceAccum = glm::vec3(0.0f);
            }
    };

};

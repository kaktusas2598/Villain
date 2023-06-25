#ifndef __PARTICLE__
#define __PARTICLE__

#include <glm/glm.hpp>

namespace Villain {

    // Simplest physical body, that can be simulated, point mass object
    class Particle {
        public:

            void integrate(float duration);

            glm::vec3 getPosition() const { return position; }
            glm::vec3 getVelocity() const { return velocity; }
            glm::vec3 getAcceleration() const { return acceleration; }
            float getInverseMass() const { return inverseMass; }

            void setAcceleration(const glm::vec3& acc) { acceleration = acc; }
            void setMass(float m) { inverseMass = 1/m; }
            void setInverseMass(float m) { inverseMass = m; }

        protected:
            glm::vec3 position; //<<< Linear position in world space
            glm::vec3 velocity; //<<< Linear velocity in world space
            glm::vec3 acceleration; //<<< Linear acceleration in world space
            float inverseMass; //<<< Inverse mass is more useful in numerical integration and for simulating infinite mass bodies

            float damping; //<<< Damping applied to linear motion to simulate real world drag

        private:
            void clearAccumulator() {
                acceleration = glm::vec3(0.0f);
            }
    };

};

#endif // __PARTICLE__

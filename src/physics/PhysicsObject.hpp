#ifndef __PHYSICS_OBJECT__
#define __PHYSICS_OBJECT__

#include "glm/glm.hpp"
#include "physics/BoundingSphere.hpp"

namespace Villain {

    // Super basic physics object
    class PhysicsObject {
        public:
            PhysicsObject(const glm::vec3& pos, const glm::vec3& vel, float r) :
                position(pos), velocity(vel), radius(r) {}

            void integrate(float deltaTime);

            inline const glm::vec3 getPosition() const { return position; }
            inline const glm::vec3 getVelocity() const { return velocity; }
            inline float getRadius() const { return radius; }
            inline BoundingSphere getBoundingSphere() const {
                return BoundingSphere(position, radius);
            }

            inline void setVelocity(const glm::vec3& vel) { velocity = vel; }
        private:
            glm::vec3 position{0.f, 0.f, 0.f};
            glm::vec3 velocity{0.f, 0.f, 0.f};
            float radius; // NOTE: for now Physics Objects will all act as bounding spheres
    };
}

#endif // __PHYSICS_OBJECT__

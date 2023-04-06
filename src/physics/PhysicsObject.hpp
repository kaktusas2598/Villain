#ifndef __PHYSICS_OBJECT__
#define __PHYSICS_OBJECT__

#include "glm/glm.hpp"

namespace Villain {

    // Super basic physics object
    class PhysicsObject {
        public:
            PhysicsObject(const glm::vec3& pos, const glm::vec3& vel) :
                position(pos), velocity(vel) {}

            void integrate(float deltaTime);

            inline const glm::vec3 getPosition() const { return position; }
            inline const glm::vec3 getVelocity() const { return velocity; }
        private:
            glm::vec3 position{0.f, 0.f, 0.f};
            glm::vec3 velocity{0.f, 0.f, 0.f};
    };
}

#endif // __PHYSICS_OBJECT__

#ifndef __PHYSICS_OBJECT__
#define __PHYSICS_OBJECT__

#include "Collider.hpp"

namespace Villain {

    // Super basic physics object
    class PhysicsObject {
        public:
            PhysicsObject(Collider* col, const glm::vec3& vel) :
                position(col->getCentre()), oldPosition(position), velocity(vel), collider(col) {}
            PhysicsObject(const PhysicsObject& other);
            PhysicsObject operator=(PhysicsObject other);
            virtual ~PhysicsObject() {}

            void integrate(float deltaTime);

            inline const glm::vec3 getPosition() const { return position; }
            inline const glm::vec3 getVelocity() const { return velocity; }

            inline const Collider& getCollider() {
                glm::vec3 translation = position - oldPosition;
                oldPosition = position;
                collider->transform(translation);

                return *collider;
            }

            inline void setVelocity(const glm::vec3& vel) { velocity = vel; }
        private:
            glm::vec3 position{0.f, 0.f, 0.f};
            glm::vec3 oldPosition;
            glm::vec3 velocity{0.f, 0.f, 0.f};

            Collider* collider = nullptr;
    };
}

#endif // __PHYSICS_OBJECT__

#ifndef __PHYSICS_OBJECT__
#define __PHYSICS_OBJECT__

#include "Collider.hpp"

namespace Villain {

    // Super basic physics object
    // TODO: Might want to make this abstract class and implement a few different types of physics objects:
    // Rigid Bodies, Static obstacles, Triggers
    class PhysicsObject {
        public:
            PhysicsObject(Collider* col, float m = 1.0f, const glm::vec3& vel = glm::vec3(0.0f)) :
                position(col->getCentre()), oldPosition(position), velocity(vel), mass(m), collider(col) {}
            PhysicsObject(const PhysicsObject& other);
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
            bool isDynamic() const { return mass > 0.0f; }
            inline void applyForce(const glm::vec3& F) { force += F; }
        private:
            glm::vec3 position{0.f, 0.f, 0.f};
            glm::vec3 oldPosition;
            glm::vec3 velocity{0.f, 0.f, 0.f};

            Collider* collider = nullptr;

            // Rigid Body specific
            float mass = 1.0f; // static objects have mass of zero
            glm::vec3 force;
            glm::vec3 linearVelocity;
            glm::vec3 angularVelocity;
            float torque;
            // TODO:
            //   - RigidBody class extending this one(rename this to CollisionObject)
            //   - quaternions
            //   - friction
            //   - restitution
    };
}

#endif // __PHYSICS_OBJECT__

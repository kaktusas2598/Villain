#pragma once

#include "BoundingVolume.hpp"
#include "glm/gtx/quaternion.hpp"

namespace Villain {

    // Super basic physics object
    // TODO: Might want to make this abstract class and implement a few different types of physics objects:
    // Rigid Bodies, Static obstacles, Triggers
    class PhysicsObject {
        public:
            PhysicsObject(BoundingVolume* col, float m = 1.0f, const glm::vec3& vel = glm::vec3(0.0f)) :
                position(col->getCentre()), oldPosition(position), linearVelocity(vel), mass(m), collider(col),
                force(0.0f) {
                    // ? Is this correct ?
                    inverseMass = (mass > 0.0) ? 1.f/mass : mass;
                }
            PhysicsObject(const PhysicsObject& other);
            //virtual ~PhysicsObject() {}
            ~PhysicsObject() {}

            void integrate(float deltaTime);

            inline const glm::vec3 getPosition() const { return position; }
            inline const glm::vec3 getVelocity() const { return linearVelocity; }

            inline const BoundingVolume& getCollider() {
                glm::vec3 translation = position - oldPosition;
                oldPosition = position;
                collider->transform(translation);

                return *collider;
            }

            //void recalculate() {
                //velocity = momentum * velocity;
            //}

            inline void setVelocity(const glm::vec3& vel) { linearVelocity = vel; }
            bool isDynamic() const { return mass > 0.0f; }
            inline void applyForce(const glm::vec3& F) { force += F; }
            void clearForces() { force = glm::vec3(0.0f); }
        private:
            // primary
            glm::vec3 position{0.f, 0.f, 0.f};
            glm::vec3 oldPosition;
            glm::vec3 momentum{0.0, 0.0f, 0.0f}; // does it make sense to initialise momentum to zero?

            // secondary
            glm::vec3 linearVelocity{0.f, 0.f, 0.f};

            BoundingVolume* collider = nullptr;

            // TODO: Implement all of this, possibly refactor into new RigidBody class extending this one
            // Rigid Body specific
            float mass = 1.0f; // static objects have mass of zero
            float inverseMass;

            glm::vec3 force;

            glm::vec3 angularVelocity;
            glm::vec3 angularMomentum;
            glm::quat orientation;
            glm::quat spin;

            // NOTE: investigate constructing inertia tensor matrixes for different collider shapes
            float inertia;
            float inverseIntertia;

            float torque;
            float momentOfIntertia; // Calculated from Collider shape, measurements and mass needed

            float friction; //<<< Friction coefficient
            float restitution; //<<< Elasticity
    };
}

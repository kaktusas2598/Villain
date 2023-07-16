#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Villain {

    class RigidBody {
        public:
            RigidBody();

            void integrate(float deltaTime);

            // Calculate internal data from state(transformMatrix, etc.), automatically called in integration, and should
            // always be called after body's state is altered directly!
            void calculateDerivedData();

            // Adds given force to the center of mass of the rigid body, bypassing torque
            void addForce(const glm::vec3& force) {
                forceAccum += force;
                isAwake = true;
            }
            // Adds given force to the given point on the rigid body, both force and point must be in world space
            // Because the force is not applied at the center of mass, it might generate torque
            void addForceAtPoint(const glm::vec3& force, const glm::vec3& point) {
                // Convert to coordinates relative to center of mass
                glm::vec3 pt = point - position;

                forceAccum += force;
                torqueAccum += glm::cross(pt, force);
                isAwake = true;
            }
            // Adds given force to the given point on the rigid body, force is in world space, but point is in body/local space
            // Useful for spring forces or other forces fixed to the body
            void addForceAtBodyPoint(const glm::vec3& force, const glm::vec3& point) {
                addForceAtPoint(force, getPointInWorldSpace(point));
            }

            bool hasFiniteMass() const { return inverseMass >= 0.0f; }

            // Getters
            float getInverseMass() const { return inverseMass; }
            glm::mat3 getInverseInertiaTensor() const { return inverseInertiaTensor; }
            float getMass() const { return (inverseMass == 0.0f) ? FLT_MAX : 1.0f / inverseMass; }
            float getLinearDamping() const { return linearDamping; }
            float getAngularDamping() const { return angularDamping; }
            glm::vec3 getPosition() const { return position; }
            glm::quat getOrientation() const { return orientation; }
            glm::vec3 getLinearVelocity() const { return velocity; }
            glm::vec3 getAngularVelocity() const { return rotation; }
            glm::mat4 getTransform() const { return transformMatrix; }

            // Setters
            void setMass(float m) { inverseMass = 1/m; }
            void setInverseMass(float value) { inverseMass = value; }
            void setInertiaTensor(const glm::mat3& inertiaTensor) { inverseInertiaTensor = glm::inverse(inertiaTensor); }
            void setLinearDamping(float value) { linearDamping = value; }
            void setAngularDamping(float value) { angularDamping = value; }
            void setPosition(const glm::vec3& value) { position = value; }
            void setOrientation(const glm::quat& value) { orientation = value; }
            void setLinearVelocity(const glm::vec3& value) { velocity = value; }
            void setAngularVelocity(const glm::vec3& value) { rotation = value; }
            void setTransform(const glm::mat4& value) { transformMatrix = value; }
            void setDamping(float linear, float angular) { linearDamping = linear; angularDamping = angular; }

            // Helpers
            // TODO: Make sure these methods are correct
            glm::vec3 getPointInWorldSpace(const glm::vec3& point) {
                return transformMatrix * glm::vec4(point, 1.0);
            }
            glm::vec3 getPointInLocalSpace(const glm::vec3& point) {
                return glm::inverse(transformMatrix) * glm::vec4(point, 1.0);
            }

            // Called automatically after each integration step
            void clearAccumulators() {
                forceAccum = glm::vec3(0.0f);
                torqueAccum = glm::vec3(0.0f);
            }

        protected:
            // --- Characteristic attributes
            // Inverse mass is more useful in numerical integration and for simulating infinite mass bodies
            float inverseMass;
            // Inverse of body's inertia tensor. Given in body space unlike the other variables
            // Damping applied to linear motion, required to remove energy added due to numerical instability in the integrator
            // Provided inertia tensor must not degenerate (meaning body had zero inertia for spinning along one axis)
            glm::mat3 inverseInertiaTensor;
            float linearDamping;
            // Damping applied to angular motion, required to remove energy added due to numerical instability in the integrator
            float angularDamping;
            glm::vec3 position; //< Linear position of the rigid body in world space
            glm::quat orientation; //< Angular orientation of the rigid body in world space
            glm::vec3 velocity; //< Linear velocity of the rigid body in the world space
            glm::vec3 rotation; //< Angular velocity (here referred as rotation) of the rigid body in world space

            // --- Derived attributes
            bool isAwake; //< Body can be put to sleep to avoid being integrated or affected by collisions
            glm::mat3 inverseInertiaTensorWorld;
            glm::mat4 transformMatrix; //< Converts body space to world space, useful for rendering

            // --- Force, Torque accumulator attributes
            glm::vec3 forceAccum; //< Accumulated force applied at next integration only and cleared after
            glm::vec3 torqueAccum; //< Accumulated torque applied at next integration only and cleared after
            glm::vec3 acceleration; //< Used to set gravity or any other constant gravitation
            glm::vec3 lastFrameAcceleration; //< Linear acceleration for the previous frame

        private:
            void calculateTransformMatrix();
    };
}

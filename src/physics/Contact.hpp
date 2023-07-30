#pragma once

#include "RigidBody.hpp"

namespace Villain {

    class ContactResolver;

    /*! \brief Represent contact of 2 colliding bodies.
     *
     * Only represent contact data between 2 colliding rigid bodies, use ContactResolver
     * to resolve set of contacts.
     */
    class Contact {
        public:
            friend class ContactResolver;

            /// By default give invalid contact
            Contact() : bodies{ nullptr, nullptr }, friction(-1.0f), restitution(-1.0f),
                contactPoint(glm::vec3(-9999.0f)), contactNormal(glm::vec3(0.0f)),
                penetration(-9999.0f), contactToWorld(glm::mat3(0.0f)),
                contactVelocity(glm::vec3(0.0f)), desiredDeltaVelocity(0.0f),
                relativeContactPos{ glm::vec3(-9999.0f), glm::vec3(-9999.0f) } {}

            RigidBody* bodies[2]; ///< Bodies involved in the contact. 2nd body can be nullptr for contacts with scenery.
            float friction; ///< Lateral friction coefficient at the contact
            float restitution; ///< Normal restitution coefficient at the contact to describe response bounciness

            glm::vec3 contactPoint; ///< Position of the contact in world coordinates.
            glm::vec3 contactNormal; ///< Direction of the contact in world coordinates.
            float penetration; ///< Depth of penetration at the contact point.

            /// Set properties that don't normally depend on collision point (bodies and material properties)
            void setBodyData(RigidBody* one, RigidBody* two, float frictionCoef, float restCoef);

            // Getters for unit tests mainly
            RigidBody* getBody(unsigned index) const { return bodies[index]; }
            float getFriction() const { return friction; }
            float getRestitution() const { return restitution; }
            glm::vec3 getContactPoint() const { return contactPoint; }
            glm::vec3 getContactNormal() const { return contactNormal; }
            float getPenetration() const { return penetration; }
            glm::mat3 getContactToWorld() const { return contactToWorld; }
            glm::vec3 getContactVelocity() const { return contactVelocity; }
            float getDesiredDeltaVelocity() const { return desiredDeltaVelocity; }
            glm::vec3 getRelativeContactPos(unsigned index) const { return relativeContactPos[index]; }

            // Setters for unit tests mainly
            void setContactPoint(const glm::vec3& point) { contactPoint = point; }
            void setContactNormal(const glm::vec3& normal) { contactNormal = normal; }
            void setPenetration(float pen) { penetration = pen; }

            /// Should be called automatically before resolution algorithm is run
            void calculateInternals(float deltaTime);
        protected:
            /// Reverses the contact, calculateInternals() must be called after doing this!
            void swapBodies();
            /// Update awake state of bodies that are in contact
            void matchAwakeState();
            /// Calculate internal value for the desired delta velocity
            void calculateDesiredDeltaVelocity(float deltaTime);
            /// Caclulates the velocity of the contact point at the given body
            glm::vec3 calculateLocalVelocity(unsigned bodyIndex, float deltaTime);
            /// Calculates an orthonormal basis 3x3 matrix for the contact point, based on the primary
            /// friction direction (for anisotropic friction) or a random direction(for isotropic friction)
            void calculateContactBasis();

            /// Perform inertia-weighted impule based resolution of this contact alone
            void applyVelocityChange(glm::vec3 linearVelChange[2], glm::vec3 angularVelChange[2]);
            /// Perform inertia-weighted penetration resolution of this contact alone
            void applyPositionChange(glm::vec3 linearChange[2], glm::vec3 angularChange[2], float penetration);

            /// Calculate the impulse needed to resolve this contact given that the contact has no friction
            glm::vec3 calculateFrictionlessImpulse(glm::mat3* inverseInertiaTensor);
            /// Calculate the impulse needed to resolve this contact given that the contact has friction
            glm::vec3 calculateFrictionImpulse(glm::mat3* inverseInertiaTensor);

            /// Converts local contact space coordinates to world space. Columns of this matrix form
            /// an orthonormal set of vectors
            glm::mat3 contactToWorld;

            glm::vec3 contactVelocity; ///< Closing velocity at the point of contact
            float desiredDeltaVelocity; ///< Required change in velocity to resolve this contact
            glm::vec3 relativeContactPos[2]; ///< World-space position of contact point relative to center of each body
        private:
    };

    /*! \brief Contact resolution routine
     *
     * Contact resolver instance can be shared for the entire physics simulation.
     */
    class ContactResolver {
        public:
            ContactResolver(unsigned iterations, float velEpsilon = 0.01f, float posEpsilon = 0.01f)
                : velocityIterations(iterations), positionIterations(iterations),
                velocityEpsilon(velEpsilon), positionEpsilon(posEpsilon) {}

            void setIterations(unsigned iterations) {
                velocityIterations = iterations;
                positionIterations = iterations;
            }

            /// Resolves set of contacts for both velocity and penetration. Contacts that cannot intersect
            /// with each other, should be pased to separate calls of resolveContacts, better to do many calls
            /// with smaller sets.
            void resolveContacts(Contact* contactArray, unsigned numContacts, float deltaTime);

            unsigned velocityIterations; ///< Number of iterations to performe when resolving velocity
            unsigned positionIterations; ///< Number of iterations to performe when resolving position

            float velocityEpsilon = 0.01f; ///< To reduce instability
            float positionEpsilon = 0.01f; ///< To reduce instability

            unsigned velocityIterationsUsed; ///< Number of velocity iterations used in the last call to resolve contacts
            unsigned positionIterationsUsed; ///< Number of position iterations used in the last call to resolve contacts

        protected:
            /// Calculate internal data for contacts and make sure correct bodies are alive
            void prepareContacts(Contact* contactArray, unsigned numContacts, float deltaTime);

            /// Resolves velocity issues for a given array of constraints, using given number of iterations
            void adjustVelocities(Contact* contactArray, unsigned numContacts, float deltaTime);

            /// Resolves positional issues for a given array of constraints, using given number of iterations
            void adjustPositions(Contact* contactArray, unsigned numContacts, float deltaTime);
    };

    /// Interface for contact generators which apply to to rigid bodies
    class ContactGenerator {
        public:
            /// Fills given contact structure with generated contact. Contact pointer should point to
            /// the first available contact in contact array, where limit is the maximum number of contacts that
            /// can be written to. Returns number of contacts written to
            virtual unsigned addContact(Contact* contact, unsigned limit) const = 0;
    };
}

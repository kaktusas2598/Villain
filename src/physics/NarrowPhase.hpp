#pragma once

#include "Contact.hpp"
#include "RigidBody.hpp"
#include <glm/gtc/matrix_access.hpp>

namespace Villain {

    class CollisionDetector;
    class IntersectionTests;

    /// Represents primitive to detect collision against
    class CollisionPrimitive {
        public:
            friend class CollisionDetector;
            friend class IntersectionTests;

            CollisionPrimitive(RigidBody* body, glm::mat4 offset = glm::mat4(1.0f))
                : body(body), offset(offset) {
                    if (body) calculateTransform();
                }

            RigidBody* body; ///< Rigid body represented by this primitive
            glm::mat4 offset; ///< Offset matrix for this primitive from the give rigid body

            void calculateTransform() {
                transform = body->getTransform() * offset;
            }

            /// Allow access to the axis vectors in the transform of this primitive
            glm::vec3 getAxis(unsigned index) const {
                return glm::column(transform, index);
            }

            const glm::mat4 getTransform() const { return transform; }

        protected:
            /// Resultant transform of the primitive, calculated by combining offset of the primitive
            /// by the transform of the rigid body
            glm::mat4 transform;
    };

    /// Represents a rigid body that can be treated as a sphere for collision detection
    class CollisionSphere : public CollisionPrimitive {
        public:
            CollisionSphere(float radius, RigidBody* body, glm::mat4 offset = glm::mat4(1.0f))
                : radius(radius), CollisionPrimitive(body, offset) {}

            float radius; ///< Radius of the sphere
    };

    /// Plane here is not a primitive! It does not represent rigid bodies, but is used for contacts with world geometry
    class CollisionPlane : public CollisionPrimitive {
        public:
            CollisionPlane(glm::vec3 normal, float distance, glm::mat4 offset = glm::mat4(1.0f))
                : direction(normal), offset(distance), CollisionPrimitive(nullptr, offset) {}

            glm::vec3 direction; ///< Plane normal
            float offset; ///< Distance of the plane from the origin
    };

    /// Represents a rigid body that can be treated as an axis-aligned box for collision detection
    class CollisionBox : public CollisionPrimitive {
        public:
            CollisionBox(glm::vec3 half, RigidBody* body, glm::mat4 offset = glm::mat4(1.0f))
                : halfSize(half), CollisionPrimitive(body, offset) {}

            glm::vec3 halfSize; ///< Half sizes of the box along its local axes
    };

    /// Fast intersection tests to use as an early exit in narrow phase collision detection
    class IntersectionTests {
        public:
            static bool boxAndHalfSpace(const CollisionBox& box, const CollisionPlane& plane);
    };

    /// Helper structure that contains information for the detector to be used when building contacts
    struct CollisionData {

        Contact* contacts; ///< Contact array to write to
        int contactsLeft; ///< Maximum number of contacts the array can take
        unsigned contactCount = 0; ///< Number of contacts found so far

        float friction; ///< Friction cofficient to write into any collisions
        float restitution; ///< Resitution(bounciness) coefficient to write into any collisions

        /// Updates contact array and number of added/left contacts
        void addContacts(unsigned count) {
            // Reduce number of contacts remaining, add to number of contacts used
            contactsLeft -= count;
            contactCount += count;

            // Move contact pointer forward
            contacts += count;
        }

    };

    /// Holds fine grained, narrow phase collision detection routines
    class CollisionDetector {
        public:
            static unsigned sphereAndSphere(const CollisionSphere& one, const CollisionSphere& two, CollisionData* data);
            static unsigned sphereAndHalfSpace(const CollisionSphere& sphere, const CollisionPlane& plane, CollisionData* data);
            static unsigned sphereAndTruePlane(const CollisionSphere& sphere, const CollisionPlane& plane, CollisionData* data);

            // TODO: Implement and test boxAndBox using SAT!
            static unsigned boxAndHalfSpace(const CollisionBox& box, const CollisionPlane& plane, CollisionData* data);
            static unsigned boxAndSphere(const CollisionBox& box, const CollisionSphere& sphere, CollisionData* data);
    };
}

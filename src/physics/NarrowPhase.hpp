#pragma once

#include "Contact.hpp"
#include "RigidBody.hpp"
#include "rendering/DebugRenderer.hpp"
#include <glm/gtc/matrix_access.hpp>

namespace Villain {

    // Forward declarations
    class CollisionPrimitive;
    class CollisionDetector;
    class IntersectionTests;

    /// Used primarily for raycasting
    struct Ray {
        glm::vec3 From;    ///< Ray's starting position
        glm::vec3 To;      ///< Ray's ending position

        Ray(const glm::vec3& from, const glm::vec3& to): From(from), To(to) {}
        glm::vec3 getDirection() const { return glm::normalize(To - From); }
    };

    /// Stores information about a ray hitting primitive
    struct RayHitResult {
        bool hit;          ///< True if the ray intersects with the primitive
        glm::vec3 point;   ///< The point of intersection with the primitive
        glm::vec3 normal;  ///< The normal at the point of intersection
        float distance;    ///< The distance from the ray origin to the intersection point
        CollisionPrimitive* collisionObject;
    };

    /// Represents primitive to detect collision against
    class CollisionPrimitive {
        public:
            friend class CollisionDetector;
            friend class IntersectionTests;

            CollisionPrimitive(RigidBody* body, glm::mat4 offset = glm::mat4(1.0f))
                : body(body), offsetMatrix(offset) {
                    calculateTransform();
                }
            virtual ~CollisionPrimitive() {}

            RigidBody* body; ///< Rigid body represented by this primitive
            glm::mat4 offsetMatrix; ///< Offset matrix for this primitive from the give rigid body

            void calculateTransform() {
                if (body) {
                    if (body->getInverseMass() <= 0.0f)
                        body->calculateDerivedData();
                    transform = body->getTransform() * offsetMatrix;
                } else {
                    transform = offsetMatrix;
                }
            }

            /// Allow access to the axis vectors in the transform of this primitive
            glm::vec3 getAxis(unsigned index) const {
                return glm::column(transform, index);
            }

            const glm::mat4 getTransform() const { return transform; }

            virtual bool intersectRay(Ray& ray, RayHitResult& result) const = 0;
            virtual void debugDraw(DebugRenderer* renderer) const = 0;

            const std::string& getName() const {
                return name;
            }

        protected:
            /// Resultant transform of the primitive, calculated by combining offset of the primitive
            /// by the transform of the rigid body
            glm::mat4 transform;

            std::string name; // The auto-generated name of the collider
    };

    /// Represents a rigid body that can be treated as a sphere for collision detection
    class CollisionSphere : public CollisionPrimitive {
        public:
            CollisionSphere(float radius, RigidBody* body, glm::mat4 offset = glm::mat4(1.0f))
                : radius(radius), CollisionPrimitive(body, offset) {
                    generateName();
                }

            float radius; ///< Radius of the sphere

            virtual bool intersectRay(Ray& ray, RayHitResult& result) const override;
            virtual void debugDraw(DebugRenderer* renderer) const override {
                renderer->drawSphere(getAxis(3), radius);
            };
            void generateName() {
                static int instanceCount = 0;
                name = "Sphere " + std::to_string(++instanceCount);
            }
    };

    /// Plane here is not a primitive! It does not represent rigid bodies, but is used for contacts with world geometry
    class CollisionPlane : public CollisionPrimitive {
        public:
            CollisionPlane(glm::vec3 normal, float distance, glm::mat4 offsetM = glm::mat4(1.0f))
                : direction(normal), offset(distance), CollisionPrimitive(nullptr, offsetM) {
                    generateName();
                }

            glm::vec3 direction; ///< Plane normal
            float offset; ///< Distance of the plane from the origin

            virtual bool intersectRay(Ray& ray, RayHitResult& result) const override;
            virtual void debugDraw(DebugRenderer* renderer) const override {
                // NOTE:What size to use here? Possibly need to draw 'infinite' grid here
                renderer->drawPlane(direction, offset, {5.0f, 5.0f});
            };
            void generateName() {
                static int instanceCount = 0;
                name = "Plane " + std::to_string(++instanceCount);
            }
    };

    /// Represents a rigid body that can be treated as an axis-aligned box for collision detection
    class CollisionBox : public CollisionPrimitive {
        public:
            CollisionBox(glm::vec3 half, RigidBody* body, glm::mat4 offset = glm::mat4(1.0f))
                : halfSize(half), CollisionPrimitive(body, offset) {
                    generateName();
                }

            glm::vec3 halfSize; ///< Half sizes of the box along its local axes

            virtual bool intersectRay(Ray& ray, RayHitResult& result) const override;
            virtual void debugDraw(DebugRenderer* renderer) const override {
                renderer->drawBox3DRotated(getAxis(3), halfSize * 2.0f, glm::mat4(glm::mat3(transform)));
            };
            void generateName() {
                static int instanceCount = 0;
                name = "Box " + std::to_string(++instanceCount);
            }
    };

    /// Fast intersection tests to use as an early exit in narrow phase collision detection
    class IntersectionTests {
        public:
            static bool boxAndHalfSpace(const CollisionBox& box, const CollisionPlane& plane);
    };

    /// Helper structure that contains information for the detector to be used when building contacts
    struct CollisionData {

        CollisionData(Contact* contactArray, int contactsRem, unsigned contactCnt, float rest, float frict = 0.0f)
            : contacts(contactArray), contactsLeft(contactsRem), contactCount(contactCnt), friction(frict), restitution(rest) {}

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

            static unsigned boxAndHalfSpace(const CollisionBox& box, const CollisionPlane& plane, CollisionData* data);
            static unsigned boxAndSphere(const CollisionBox& box, const CollisionSphere& sphere, CollisionData* data);
            static unsigned boxAndBox(const CollisionBox& one, const CollisionBox& two, CollisionData* data);
    };
}

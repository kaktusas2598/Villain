#pragma once

#include "RigidBody.hpp"

namespace Villain {

    /*! \brief Represent contact of 2 colliding bodies.
     *
     * Only represent contact data between 2 colliding rigid bodies, use ContactResolver
     * to resolve set of contacts.
     */
    class Contact {
        public:
            RigidBody* bodies[2]; ///< Bodies involved in the contact. 2nd body can be nullptr for contacts with scenery.
            float friction; ///< Lateral friction coefficient at the contact
            float restituion; ///< Normal restitution coefficient at the contact to describe response bounciness

            glm::vec3 contactPoint; ///< Position of the contact in world coordinates.
            glm::vec3 contactNormal; ///< Direction of the contact in world coordinates.
            float penetration; ///< Depth of penetration at the contact point.

            /// Set properties that don't normally depend on collision point (bodies and material properties)
            void setBodyData(RigidBody* one, RigidBody* two, float frictionCoef, float restCoef);

            /// Calculates an orthonormal basis 3x3 matrix for the contact point, based on the primary
            /// friction direction (for anisotropic friction) or a random direction(for isotropic friction)
            void calculateContactBasis();

        protected:
            /// Converts local contact space coordinates to world space. Columns of this matrix form
            /// an orthonormal set of vectors
            glm::mat3 contactToWorld;
        private:
    };

    // TODO:
    class ContactResolver {};
}

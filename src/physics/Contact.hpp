#pragma once

#include "RigidBody.hpp"

// TODO: implement
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

            /// Set properties that doesn't normally depend on collision point (bodies and material properties)
            void setBodyData(RigidBody* one, RigidBody* two, float frictionCoef, float restCoef);

        protected:
        private:
    };

    // TODO:
    class ContactResolver {};
}

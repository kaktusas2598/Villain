#include "Contact.hpp"

namespace Villain {

    void Contact::setBodyData(RigidBody* one, RigidBody* two, float frictionCoef, float restCoef) {
        bodies[0] = one;
        bodies[1] = two;
        friction = frictionCoef;
        restituion = restCoef;
    }

    void Contact::calculateContactBasis() {
        glm::vec3 contactTangent[2];

        // Check whether Z-axis is nearer to X or Y axis
        if (fabs(contactNormal.x) > fabs(contactNormal.y)) {
            // Scaling factor ensures the results are normalised
            const float s = 1.0f / sqrtf(contactNormal.z * contactNormal.z + contactNormal.x * contactNormal.x);

            // New X-axis is at right angle to the world Y axis
            contactTangent[0].x = contactNormal.z * s;
            contactTangent[0].y = 0;
            contactTangent[0].z = -contactNormal.x * s;

            // New Y-axis is at right angles to the new X and Z axes
            contactTangent[1].x = contactNormal.y * contactTangent[0].x;
            contactTangent[1].y = contactNormal.z * contactTangent[0].x - contactNormal.x * contactTangent[0].z;
            contactTangent[1].z = -contactNormal.y * contactTangent[0].x;
        } else {
            // Scaling factor ensures the results are normalised
            const float s = 1.0f / sqrtf(contactNormal.z * contactNormal.z + contactNormal.y * contactNormal.y);

            // New X-axis is at right angle to the world X axis
            contactTangent[0].x = 0;
            contactTangent[0].y = -contactNormal.z * s;
            contactTangent[0].z = contactNormal.y * s;

            // New Y-axis is at right angles to the new X and Z axes
            contactTangent[1].x = contactNormal.y * contactTangent[0].z- contactNormal.z * contactTangent[0].y;
            contactTangent[1].y = -contactNormal.x * contactTangent[0].z;
            contactTangent[1].z = contactNormal.x * contactTangent[0].y;
        }

        contactToWorld = glm::mat3(contactNormal, contactTangent[0], contactTangent[1]);
    }
}

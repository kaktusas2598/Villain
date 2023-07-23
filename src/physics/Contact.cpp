#include "Contact.hpp"

namespace Villain {

    void Contact::setBodyData(RigidBody* one, RigidBody* two, float frictionCoef, float restCoef) {
        bodies[0] = one;
        bodies[1] = two;
        friction = frictionCoef;
        restituion = restCoef;
    }

    void Contact::calculateInternals(float deltaTime) {
        // Check if the first object is null and swap it
        if (!bodies[0]) swapBodies();
        assert(bodies[0]);

        // Calculate a set of axes at the contact point
        calculateContactBasis();

        // Store the relative position of the contact relative to each body
        relativeContactPos[0] = contactPoint - bodies[0]->getPosition();
        if (bodies[1]) {
            relativeContactPos[1] = contactPoint - bodies[1]->getPosition();
        }

        // Find the relative velocity of the bodies at the contact point
        contactVelocity = calculateLocalVelocity(0, deltaTime);
        if (bodies[1]) {
            contactVelocity -= calculateLocalVelocity(1, deltaTime);
        }

        // Calculate the desired change in velocity for resolution
        calculateDesiredDeltaVelocity(deltaTime);
    }

    void Contact::swapBodies() {
        contactNormal *= -1.0f;

        RigidBody* temp = bodies[0];
        bodies[0] = bodies[1];
        bodies[1] = temp;
    }

    void Contact::matchAwakeState() {
        // Collision with immovable object won't cause body to wake up
        if (!bodies[1]) return;

        bool body0Awake = bodies[0]->getAwake();
        bool body1Awake = bodies[1]->getAwake();

        // Wake up only the sleeping one
        if (body0Awake ^ body1Awake) {
            if (body0Awake) bodies[1]->setAwake();
            else bodies[0]->setAwake();
        }
    }

    void Contact::calculateDesiredDeltaVelocity(float deltaTime) {
        const static float velocityLimit = 0.25f;

        // Calculate the acceleration induced velocity accumulated this frame
        float velocityFromAcc = 0;

        if (bodies[0]->getAwake()) {
            velocityFromAcc += glm::dot(bodies[0]->getLastFrameAcceleration() * deltaTime, contactNormal);
        }
        if (bodies[1] && bodies[1]->getAwake()) {
            velocityFromAcc -= glm::dot(bodies[1]->getLastFrameAcceleration() * deltaTime, contactNormal);
        }

        // If velocity is very slow, limit the restitution
        float thisRestitution = restituion;
        if (fabs(contactVelocity.x) < velocityLimit) {
            thisRestitution = 0.0f;
        }

        // Combine the bounce velocity with the removed acceleration velocity
        desiredDeltaVelocity = -contactVelocity.x - thisRestitution * (contactVelocity.x - velocityFromAcc);
    }

    glm::vec3 Contact::calculateLocalVelocity(unsigned bodyIndex, float deltaTime) {
        RigidBody* thisBody = bodies[bodyIndex];

        // Work out the velocity of the contact point
        glm::vec3 velocity = glm::cross(thisBody->getAngularVelocity(), relativeContactPos[bodyIndex]);
        velocity += thisBody->getLinearVelocity();

        // Turn the velocity into contact coordinates
        glm::vec3 contactVelocity = glm::transpose(contactToWorld) * velocity;

        // Calculate the amount of velocity that is due to forces without reactions
        glm::vec3 accVelocity = thisBody->getLastFrameAcceleration() * deltaTime;
        contactVelocity += accVelocity;

        return contactVelocity;
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

    void Contact::applyImpulse(const glm::vec3& impulse, RigidBody* body, glm::vec3* linearVelChange, glm::vec3* angularVelChange) {
        // TODO:
    }

    void Contact::applyVelocityChange(glm::vec3 linearVelChange[2], glm::vec3 angularVelChange[2]) {
        // Get inverse mass and inverse inertia tensor in world coords
        glm::mat3 inverseInertiaTensor[2];
        inverseInertiaTensor[0] = bodies[0]->getInverseInertiaTensorWorld();
        if (bodies[1])
            inverseInertiaTensor[1] = bodies[1]->getInverseInertiaTensorWorld();

        // Will calculate impulse for each contact axis
        glm::vec3 impulseContact;

        if (friction == 0.0f) {
            // Calculate short format for frictionless impulse
            impulseContact = calculateFrictionlessImpulse(inverseInertiaTensor);
        } else {
            // TODO:friction impulse
            assert(false);
        }

        // Convert impulse to world coords:
        glm::vec3 impulse = contactToWorld * impulseContact;

        // Split impulse into linear and angular/rotational components
        glm::vec3 impulsiveTorque = glm::cross(relativeContactPos[0], impulse);
        angularVelChange[0] = inverseInertiaTensor[0] * impulsiveTorque;
        linearVelChange[0] = glm::vec3(0.0);
        linearVelChange[0] += impulse * bodies[0]->getInverseMass();

        // Apply the changes to rigid body
        bodies[0]->addLinearVelocity(linearVelChange[0]);
        bodies[0]->addAngularVelocity(angularVelChange[0]);

        if (bodies[1]) {
            // Split impulse into linear and angular/rotational components
            impulsiveTorque = glm::cross(relativeContactPos[1], impulse);
            angularVelChange[1] = inverseInertiaTensor[1] * impulsiveTorque;
            linearVelChange[1] = glm::vec3(0.0);
            linearVelChange[1] += impulse * bodies[1]->getInverseMass();

            // Apply the changes to rigid body
            bodies[1]->addLinearVelocity(linearVelChange[1]);
            bodies[1]->addAngularVelocity(angularVelChange[1]);
        }
    }

    glm::vec3 Contact::calculateFrictionlessImpulse(glm::mat3* inverseInertiaTensor) {
        glm::vec3 impulseContact;

        // Calculate vector that shows the change of velicity in world space for a unit impulse
        // in the direction of the contact normal
        glm::vec3 deltaVelWorld = glm::cross(relativeContactPos[0], contactNormal);
        deltaVelWorld = inverseInertiaTensor[0] * deltaVelWorld;
        deltaVelWorld = glm::cross(deltaVelWorld, contactNormal);

        // Find change in velocity in contact coords
        float deltaVelocity = glm::dot(deltaVelWorld, contactNormal);

        // Add linear component to change
        deltaVelocity += bodies[0]->getInverseMass();

        if (bodies[1]) {
            // Same transform sequence again
            deltaVelWorld = glm::cross(relativeContactPos[1], contactNormal);
            deltaVelWorld = inverseInertiaTensor[1] * deltaVelWorld;
            deltaVelWorld = glm::cross(deltaVelWorld, contactNormal);

            deltaVelocity = glm::dot(deltaVelWorld, contactNormal);
            deltaVelocity += bodies[1]->getInverseMass();
        }

        // Calculate the required size of the impulse
        impulseContact.x = desiredDeltaVelocity / deltaVelocity;
        impulseContact.y = 0;
        impulseContact.z = 0;
        return impulseContact;
    }

    void Contact::applyPositionChange(glm::vec3 linearChange[2], glm::vec3 angularChange[2], float penetration) {
        const float angularLimit = 0.2f;
        float angularMove[2];
        float linearMove[2];

        float totalInertia = 0;
        float linearInertia[2];
        float angularInertia[2];

        // Work out the inertia of each object in the direction of the contact normal, due to angular inertia only
        for (unsigned i = 0; i < 2; i++) {
            if (bodies[i]) {
                glm::mat3 inverseInertiaTensor = bodies[i]->getInverseInertiaTensorWorld();

                glm::vec3 angularInertiaWorld = glm::cross(relativeContactPos[i], contactNormal);
                angularInertiaWorld = inverseInertiaTensor * angularInertiaWorld;
                angularInertia[i] = glm::dot(angularInertiaWorld, contactNormal);

                // Linear component is just inverse mass
                linearInertia[i] = bodies[i]->getInverseMass();

                // Keep track of total inertia forom all components;
                totalInertia += linearInertia[i] + angularInertia[i];

                // Break the loop here so that totalInertia value is completely calculated
                // by both iterations before continuing
            }
        }

        // Loop again calculating and applying the changes
        for (unsigned i = 0; i < 2; i++) {
            if (bodies[i]) {
                // Linear and angular movements are proportional to the two inverse inertias
                float sign = (i == 0) ? 1 : -1;
                angularMove[i] = sign * penetration * (angularInertia[i] / totalInertia);
                linearMove[i] = sign * penetration * (linearInertia[i] / totalInertia);

                // To avoid angular projections that are too great (when mass is large but inertia tensor is small)
                // limit the angular move
                glm::vec3 projection = relativeContactPos[i];
                projection += contactNormal * (-glm::dot(relativeContactPos[i], contactNormal));

                // Use the small angle approximation for the sine of the angle
                float maxMagnitude = angularLimit * glm::length(projection);

                if (angularMove[i] < -maxMagnitude) {
                    float totalMove = angularMove[i] + linearMove[i];
                    angularMove[i] = -maxMagnitude;
                    linearMove[i] = totalMove - angularMove[i];
                } else if (angularMove[i] > maxMagnitude) {
                    float totalMove = angularMove[i] + linearMove[i];
                    angularMove[i] = maxMagnitude;
                    linearMove[i] = totalMove - angularMove[i];
                }

                // Now we have a linear amount of movement required by turning rigid body(angularMove[i])
                // So need to calculate the desired rotation to achieve that
                if (angularMove[i] == 0) {
                    // No angular movement - so no rotation
                    angularChange[i] = glm::vec3(0.0);
                } else {
                    // Work out the direction to rotate in
                    glm::vec3 targetAngularDir = glm::cross(relativeContactPos[i], contactNormal);

                    glm::mat3 inverseInertiaTensor = bodies[i]->getInverseInertiaTensorWorld();

                    // Work out the direction we'd need to rotate to achieve that
                    angularChange[i] = inverseInertiaTensor * targetAngularDir * (angularMove[i] / angularInertia[i]);
                }

                // Linear velocity change is easier - it's just inear movement along the contact normal
                linearChange[i] = contactNormal * linearMove[i];

                // Now we can apply values we calculated to the rigid body
                // Apply the linear movement
                glm::vec3 pos = bodies[i]->getPosition();
                pos += linearChange[i];
                bodies[i]->setPosition(pos);

                // Apply the orientation
                glm::quat q = bodies[i]->getOrientation();
                // NOTE: is this correct?
                q += glm::quat(angularChange[i]);
                bodies[i]->setOrientation(q);

                // Finally need to calculate derived data for any body that is asleep, to reflect
                // the changes in the object's data, otherwise resolution won't change object's position
                // and the next collision detection will have the same penetration
                if (!bodies[i]->getAwake()) bodies[i]->calculateDerivedData();
            }
        }
    }

    void ContactResolver::resolveContacts(Contact* contactArray, unsigned numContacts, float deltaTime) {
        // Make sure there are contacts
        if (numContacts == 0) return;

        // Prepare the contacts for processing
        prepareContacts(contactArray, numContacts, deltaTime);

        // Resolve the interpenetration problems with the contacts
        adjustPositions(contactArray, numContacts, deltaTime);

        // Resolve the velocoty problems with the contacts
        adjustVelocities(contactArray, numContacts, deltaTime);
    }

    void ContactResolver::prepareContacts(Contact* contactArray, unsigned numContacts, float deltaTime) {
        // Generate contact velocity and axis information
        Contact* lastContact = contactArray + numContacts;
        for (Contact* contact = contactArray; contact < lastContact; contact++) {
            // Calculate internal contact data (inertia, basis, etc.)
            contact->calculateInternals(deltaTime);
        }
    }

    void ContactResolver::adjustVelocities(Contact* contactArray, unsigned numContacts, float deltaTime) {
        glm::vec3 linearVelChange[2], angularVelChange[2];
        glm::vec3 deltaVel;

        // Iteratively resolv interpenetrations in order of severity
        velocityIterationsUsed = 0;
        while (velocityIterationsUsed < velocityIterations) {
            // Find contact with maximum magnitude  probable velocity change
            float max = velocityEpsilon;
            unsigned index = numContacts;
            for (unsigned i = 0; i < numContacts; i++) {
                if (contactArray[i].desiredDeltaVelocity > max) {
                    max = contactArray[i].desiredDeltaVelocity;
                    index = i;
                }
            }
            if (index == numContacts) break;

            // Match the awake state at the contact
            contactArray[index].matchAwakeState();

            // Resolve velocity
            contactArray[index].applyVelocityChange(linearVelChange, angularVelChange);

            // After changing velocity of two bodies causes some of the relative closing velocities recomputation
            for (unsigned i = 0; i < numContacts; i++) {
                // Check each body in the contact
                for (unsigned b = 0; b < 2; b++) {
                    if (contactArray[i].bodies[b]) {
                        // Check for a match with each body in the newly resolved contact
                        for (unsigned d = 0; d < 2; d++) {
                            if (contactArray[i].bodies[b] == contactArray[index].bodies[d]) {
                                deltaVel = linearVelChange[d] + glm::cross(angularVelChange[d], contactArray[i].relativeContactPos[b]);

                                // The sign of change is positive if dealing with second body in a contact, and
                                // negative otherwise
                                contactArray[i].contactVelocity +=
                                    glm::transpose(contactArray[i].contactToWorld) * deltaVel * (b ? 1.0f: -1.0f);
                                contactArray[i].calculateDesiredDeltaVelocity(deltaTime);
                            }
                        }
                    }
                }
            }
            velocityIterationsUsed++;
        }
    }

    void ContactResolver::adjustPositions(Contact* contactArray, unsigned numContacts, float deltaTime) {
        unsigned i, index;
        glm::vec3 linearChange[2], angularChange[2];
        float max;
        glm::vec3 deltaPosition;

        // Iteratively resolv interpenetrations in order of severity
        positionIterationsUsed = 0;
        while (positionIterationsUsed < positionIterations) {
            // Find biggest penetration
            max = positionEpsilon;
            index = numContacts;
            for (i = 0; i < numContacts; i++) {
                if (contactArray[i].penetration > max) {
                    max = contactArray[i].penetration;
                    index = i;
                }
            }
            if (index == numContacts) break;

            // Match the awake state at the contact
            contactArray[index].matchAwakeState();

            // Resolve the penetration
            contactArray[index].applyPositionChange(linearChange, angularChange, max);

            // This action might have changed the penetration of other bodies so need to update contacts
            for (i = 0; i < numContacts; i++) {
                // Check each body in the contact
                for (unsigned b = 0; b < 2; b++) {
                    if (contactArray[i].bodies[b]) {
                        // Check for a match with each body in the newly resolved contact
                        for (unsigned d = 0; d < 2; d++) {
                            if (contactArray[i].bodies[b] == contactArray[index].bodies[d]) {
                                deltaPosition = linearChange[d]
                                    + glm::cross(angularChange[d], contactArray[i].relativeContactPos[b]);

                                // The sign of change is positive if dealing with second body in a contact, and
                                // negative otherwise
                                contactArray[i].penetration +=
                                    glm::dot(deltaPosition, contactArray[i].contactNormal) * (b ? 1: -1);
                            }
                        }
                    }
                }
            }
            positionIterationsUsed++;
        }
    }
}

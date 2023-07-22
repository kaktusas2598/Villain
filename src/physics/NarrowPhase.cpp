#include "NarrowPhase.hpp"

#include <glm/gtx/norm.hpp>

namespace Villain {

    static inline float transformToAxis(const CollisionBox &box, const glm::vec3 &axis) {
        return
            box.halfSize.x * fabs(glm::dot(axis, box.getAxis(0))) +
            box.halfSize.y * fabs(glm::dot(axis, box.getAxis(1))) +
            box.halfSize.z * fabs(glm::dot(axis, box.getAxis(2)));
    }

    bool IntersectionTests::boxAndHalfSpace(const CollisionBox& box, const CollisionPlane& plane) {
        // Work out the projected radius of the box onto the plane direction
        float projectedRadius = transformToAxis(box, plane.direction);

        // Find box distance from the origin
        float boxDistance = glm::dot(plane.direction, box.getAxis(3)) - projectedRadius;

        // Check for intersection
        return boxDistance <= plane.offset;
    }

    unsigned CollisionDetector::sphereAndSphere(
            const CollisionSphere& one, const CollisionSphere& two, CollisionData* data)
    {
        // Make sure where are contacts available
        if (data->contactsLeft <= 0) return 0;

        // Cache sphere positions
        glm::vec3 posOne = one.getAxis(3);
        glm::vec3 posTwo = two.getAxis(3);

        // Find vector between the primitives
        glm::vec3 midline = posOne - posTwo;
        float size = glm::length(midline);

        // See if it's large enough
        if (size <= 0.0f || size >= one.radius + two.radius) {
            return 0;
        }

        // Manually create normal using size
        glm::vec3 normal = midline * 1.0f/size;

        Contact* contact = data->contacts;
        contact->contactNormal = normal;
        contact->contactPoint = posOne + midline * 0.5f;
        contact->penetration = one.radius + two.radius - size;
        contact->setBodyData(one.body, two.body, data->friction, data->restitution);

        data->addContacts(1);
        return 1;
    }

    unsigned CollisionDetector::sphereAndHalfSpace(
            const CollisionSphere& sphere, const CollisionPlane& plane, CollisionData* data)
    {
        // Make sure where are contacts available
        if (data->contactsLeft <= 0) return 0;

        // Cache the sphere position
        glm::vec3 position = sphere.getAxis(3);

        // Find the distance from the plane
        float ballDistance = glm::dot(plane.direction, position) - sphere.radius - plane.offset;

        if (ballDistance >= 0) return 0;

        // Create a contact with a normal in the plane direction
        Contact* contact = data->contacts;
        contact->contactNormal = plane.direction;
        contact->contactPoint = position - plane.direction * (ballDistance + sphere.radius);
        contact->penetration = -ballDistance;
        contact->setBodyData(sphere.body, nullptr, data->friction, data->restitution);

        data->addContacts(1);
        return 1;
    }

    unsigned CollisionDetector::sphereAndTruePlane(
            const CollisionSphere& sphere, const CollisionPlane& plane, CollisionData* data)
    {
        // Make sure where are contacts available
        if (data->contactsLeft <= 0) return 0;

        // Cache the sphere position
        glm::vec3 position = sphere.getAxis(3);

        // Find the distance from the plane
        float centreDistance = glm::dot(plane.direction, position) - plane.offset;

        // Check if we're within radius
        if (glm::dot(centreDistance, centreDistance) > sphere.radius * sphere.radius) {
            return 0;
        }

        // Check which side of the plane we're on
        glm::vec3 normal = plane.direction;
        float penetration = -centreDistance;
        if (centreDistance < 0) {
            normal *= -1;
            penetration = -penetration;
        }
        penetration += sphere.radius;

        // Create a contact with a normal in the plane direction
        Contact* contact = data->contacts;
        contact->contactNormal = normal;
        contact->contactPoint = position - plane.direction * centreDistance;
        contact->penetration = penetration;
        contact->setBodyData(sphere.body, nullptr, data->friction, data->restitution);

        data->addContacts(1);
        return 1;

    }

    unsigned CollisionDetector::boxAndHalfSpace(
            const CollisionBox& box, const CollisionPlane& plane, CollisionData* data)
    {
        // Make sure where are contacts available
        if (data->contactsLeft <= 0) return 0;

        // Early check for intersection
        if (!IntersectionTests::boxAndHalfSpace(box, plane)) {
            return 0;
        }

        // There is intersection, so we need to find intersection points by checking vertices.
        // If box is resting on a plane or on an edge, it will report four or two contact points

        // Go through each combination of + and - for each half size of the box
        static float mults[8][3] = {{1, 1, 1}, {-1, 1, 1}, {1, -1, 1}, {-1, -1, 1},
            {1, 1, -1}, {-1, 1, -1}, {1, -1, -1}, {-1, -1, -1}};

        Contact* contact = data->contacts;
        unsigned contactsUsed = 0;
        for (unsigned i = 0; i < 8; i++) {
            // Calculate the position of each vertex
            glm::vec3 vertexPos(mults[i][0], mults[i][1], mults[i][2]);
            vertexPos = vertexPos * box.halfSize;
            vertexPos = box.transform * glm::vec4(vertexPos, 1.0);

            // Calculate the distance from the plane
            float vertexDistance = glm::dot(vertexPos, plane.direction);

            // Compare this to plane's distance
            if (vertexDistance <= plane.offset) {
                // Create contact data
                // Contact point is halway between the vertex and the plane, multiply the direction
                // by half the separating distance and add vertex location
                contact->contactPoint = plane.direction;
                contact->contactPoint *= (vertexDistance - plane.offset);
                contact->contactNormal = plane.direction;
                contact->penetration = plane.offset - vertexDistance;
                contact->setBodyData(box.body, nullptr, data->friction, data->restitution);

                // Move to the next contact
                contact++;
                contactsUsed++;
                if (contactsUsed == data->contactsLeft) return contactsUsed;
            }
        }

        data->addContacts(contactsUsed);
        return contactsUsed;
    }

    unsigned CollisionDetector::boxAndSphere(const CollisionBox& box, const CollisionSphere& sphere, CollisionData* data) {
        // Transform the centre of the sphere into box coordinates
        glm::vec3 centre = sphere.getAxis(3);
        glm::vec3 realCentre = glm::inverse(box.transform) * glm::vec4(centre, 1.0);

        // Early exit intersection check
        if (fabs(realCentre.x) - sphere.radius > box.halfSize.x ||
                fabs(realCentre.y) - sphere.radius > box.halfSize.y ||
                fabs(realCentre.z) - sphere.radius > box.halfSize.z) {
            return 0;
        }

        glm::vec3 closestPt;
        float dist;

        // Clamp each coordinate to the box
        dist = realCentre.x;
        if (dist > box.halfSize.x) dist = box.halfSize.x;
        if (dist < -box.halfSize.x) dist = -box.halfSize.x;
        closestPt.x = dist;

        dist = realCentre.y;
        if (dist > box.halfSize.y) dist = box.halfSize.y;
        if (dist < -box.halfSize.y) dist = -box.halfSize.y;
        closestPt.y = dist;

        dist = realCentre.z;
        if (dist > box.halfSize.z) dist = box.halfSize.z;
        if (dist < -box.halfSize.z) dist = -box.halfSize.z;
        closestPt.z = dist;

        // Check if we're in contact
        dist = glm::length2(closestPt - realCentre);
        if (dist > sphere.radius * sphere.radius) return 0;

        // Compile the contact
        glm::vec3 closestPtWorld = box.transform * glm::vec4(closestPt, 1.0);

        Contact* contact = data->contacts;
        contact->contactNormal = glm::normalize(closestPtWorld - centre);
        contact->contactPoint = closestPtWorld;
        contact->penetration = sphere.radius - sqrtf(dist);
        contact->setBodyData(box.body, sphere.body, data->friction, data->restitution);

        data->addContacts(1);
        return 1;
    }
}

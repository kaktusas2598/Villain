#include "NarrowPhase.hpp"
#include "Logger.hpp"

#include <glm/gtx/norm.hpp>
#include <glm/gtx/string_cast.hpp>

namespace Villain {

    static inline float transformToAxis(const CollisionBox &box, const glm::vec3 &axis) {
        return
            box.halfSize.x * fabs(glm::dot(axis, box.getAxis(0))) +
            box.halfSize.y * fabs(glm::dot(axis, box.getAxis(1))) +
            box.halfSize.z * fabs(glm::dot(axis, box.getAxis(2)));
    }

    /// Project half size of each box onto the separating axis
    /// and check if two boxes are overlapping (positive) or not (negative)
    static inline float penetrationOnAxis(
            const CollisionBox& one, const CollisionBox& two, const glm::vec3& axis, const glm::vec3& toCentre)
    {
        float oneProject = transformToAxis(one, axis);
        float twoProject = transformToAxis(two, axis);

        float distance = fabs(glm::dot(toCentre, axis));

        // Return overlap (positive is overlap, negative is separation)
        return oneProject + twoProject - distance;
    }

    static inline bool tryAxis(
            const CollisionBox& one, const CollisionBox& two,
            glm::vec3 axis, const glm::vec3& toCentre,
            unsigned index,
            // May be updated
            float& smallestPenetration,
            unsigned& smallestCase)
    {
        // Make sure to normalize axis and don't check almost parallel axes
        if (glm::length2(axis) < 0.0001) return true;
        axis = glm::normalize(axis);

        float overlap = penetrationOnAxis(one, two, axis, toCentre);
        if (overlap < 0) return false;
        if (overlap < smallestPenetration) {
            smallestPenetration = overlap;
            smallestCase = index;
        }
        return true;
    }

    /// Called when there is a contact between vertex from box two and box one
    void fillPointFaceBoxBox(
            const CollisionBox& one, const CollisionBox& two,
            const glm::vec3& toCentre, CollisionData* data,
            unsigned best, float penetration) {
        Contact* contact = data->contacts;

        // We know the axis of the collision(best), but not which of the two
        // faces on this axis
        glm::vec3 normal = one.getAxis(best);
        if (glm::dot(one.getAxis(best), toCentre) > 0) {
            normal *= -1.0f;
        }

        // Work out which vertex of the box two we're colliding with (using toCentre won't work)
        glm::vec3 vertex = two.halfSize;
        if (glm::dot(two.getAxis(0), normal) < 0) vertex.x = -vertex.x;
        if (glm::dot(two.getAxis(1), normal) < 0) vertex.y = -vertex.y;
        if (glm::dot(two.getAxis(2), normal) < 0) vertex.z = -vertex.z;

        // Generate the contact data
        contact->contactNormal = normal;
        contact->penetration = penetration;
        contact->contactPoint = two.getTransform() * glm::vec4(vertex, 1.0);
        contact->setBodyData(one.body, two.body, data->friction, data->restitution);
    }

    static inline glm::vec3 contactPoint(
            const glm::vec3& pOne,
            const glm::vec3& dOne,
            float oneSize,
            const glm::vec3& pTwo,
            const glm::vec3& dTwo,
            float twoSize,
            // If this is true, and the contact point is outside the edge
            // (in case of edge-edge contact) then we use one's midpoint, otherwise we use two's
            bool useOne
            ) {
        glm::vec3 toSt, cOne, cTwo;
        float dpStaOne, dpStaTwo, dpOneTwo, smOne, smTwo;
        float denom, mua, mub;

        smOne = glm::length2(dOne);
        smTwo = glm::length2(dTwo);
        dpOneTwo = glm::dot(dTwo, dOne);

        toSt = pOne - pTwo;
        dpStaOne = glm::dot(dOne, toSt);
        dpStaTwo = glm::dot(dTwo, toSt);

        denom = smOne * smTwo - dpOneTwo * dpOneTwo;

        // Zero denoimator means parallel lines
        if (fabs(denom) < 0.0001f) return useOne ? pOne : pTwo;

        mua = (dpOneTwo * dpStaTwo - smTwo * dpStaOne) / denom;
        mub = (smOne * dpStaTwo - dpOneTwo * dpStaOne) / denom;

        // If either of the edges has the nearest point out of bounds, then the edges
        // arent's crossed and we have edge-face contact. Point is on the edge, known from
        // the useOne param
        if (mua > oneSize || mua < -oneSize || mub > twoSize || mub < -twoSize) {
            return useOne ? pOne : pTwo;
        } else {
            cOne = pOne + dOne * mua;
            cTwo = pTwo + dTwo * mub;

            return cOne * 0.5f + cTwo * 0.5f;
        }
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
        //VILLAIN_DEBUG("Sphere1 Pos {}", glm::to_string(posOne));
        //VILLAIN_DEBUG("Sphere2 Pos {}", glm::to_string(posTwo));

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

        VILLAIN_DEBUG("SphereVsSphere contact normal {}", glm::to_string(contact->contactNormal));
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
        VILLAIN_DEBUG("SphereVsPlane contact normal {}", glm::to_string(contact->contactNormal));

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
            vertexPos = vertexPos * box.halfSize; // Component product update
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
                // FIXME: was in the book's source code, but seems to causing problem in col. response!?
                contact->contactPoint += vertexPos;
                contact->contactNormal = plane.direction;
                contact->penetration = plane.offset - vertexDistance;
                contact->setBodyData(box.body, nullptr, data->friction, data->restitution);

                //VILLAIN_DEBUG("BoxVsPlane vertex position {}", glm::to_string(vertexPos));
                VILLAIN_DEBUG("BoxVsPlane contact normal {}", glm::to_string(contact->contactNormal));
                // Move to the next contact
                contact++;
                contactsUsed++;
                if (contactsUsed == (unsigned)data->contactsLeft) {
                    data->addContacts(contactsUsed);
                    return contactsUsed;
                }
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

        VILLAIN_WARN("Contact generated for box vs sphere");
        VILLAIN_DEBUG("Distance {}", dist);
        VILLAIN_DEBUG("closestPtWorld {}", glm::to_string(closestPtWorld));
        VILLAIN_DEBUG("centre {}", glm::to_string(centre));
        VILLAIN_DEBUG("BoxVsSphere contact normal {}", glm::to_string(contact->contactNormal));
        data->addContacts(1);
        return 1;
    }

    // This preprocessor definition is only used as a convenience
    // in the boxAndBox contact generation method.
#define CHECK_OVERLAP(axis, index) \
    if (!tryAxis(one, two, (axis), toCentre, (index), pen, best)) return 0;

    unsigned CollisionDetector::boxAndBox(const CollisionBox& one, const CollisionBox& two, CollisionData* data) {
        // Find vector between two centres
        glm::vec3 toCentre = two.getAxis(3) - one.getAxis(3);

        // Assume there is no contact
        float pen = FLT_MAX;
        unsigned best = 0xffffff;

        // Now check each axis returning if there is a separating axis, and otherwise
        // keeping track of axis with smallest penetration
        // Face axes for box one
        CHECK_OVERLAP(one.getAxis(0), 0);
        CHECK_OVERLAP(one.getAxis(1), 1);
        CHECK_OVERLAP(one.getAxis(2), 2);
        // Face axes for box two
        CHECK_OVERLAP(two.getAxis(0), 3);
        CHECK_OVERLAP(two.getAxis(1), 4);
        CHECK_OVERLAP(two.getAxis(2), 5);

        // Store best axis-major just in case we run into almost parallel edge collisions later
        unsigned bestSingleAxis = best;
        // Edge-edge axes
        CHECK_OVERLAP(glm::cross(one.getAxis(0), two.getAxis(0)), 6);
        CHECK_OVERLAP(glm::cross(one.getAxis(0), two.getAxis(1)), 7);
        CHECK_OVERLAP(glm::cross(one.getAxis(0), two.getAxis(2)), 8);
        CHECK_OVERLAP(glm::cross(one.getAxis(1), two.getAxis(0)), 9);
        CHECK_OVERLAP(glm::cross(one.getAxis(1), two.getAxis(1)), 10);
        CHECK_OVERLAP(glm::cross(one.getAxis(1), two.getAxis(2)), 11);
        CHECK_OVERLAP(glm::cross(one.getAxis(2), two.getAxis(0)), 12);
        CHECK_OVERLAP(glm::cross(one.getAxis(2), two.getAxis(1)), 13);
        CHECK_OVERLAP(glm::cross(one.getAxis(2), two.getAxis(2)), 14);

        // Make sure we have a result
        assert(best != 0xffffff);

        // Now that we know we have a collision and which of the axis give the smallest penetration,
        // we can deal with it in a differnt way depending on the case
        if (best < 3) {
            // Face-Vertex contact
            fillPointFaceBoxBox(one, two, toCentre, data, best, pen);
            data->addContacts(1);
            return 1;
        } else if (best < 6) {
            // Vertex-Face contact
            fillPointFaceBoxBox(two, one, toCentre*-1.0f, data, best-3, pen);
            data->addContacts(1);
            return 1;
        } else {
            // Edge-Edge contact. Find out which axes
            best -= 6;
            unsigned axisOneIndex = best / 3;
            unsigned axisTwoIndex = best % 3;
            glm::vec3 oneAxis = one.getAxis(axisOneIndex);
            glm::vec3 twoAxis = two.getAxis(axisTwoIndex);
            glm::vec3 axis = glm::normalize(glm::cross(oneAxis, twoAxis));

            // Axis should point from box one to box two
            if (glm::dot(axis, toCentre) > 0) axis *= -1.0f;

            // Now we have axes, need to get edges: each axis has 4 edges parallel to it.
            // We need to find 1 from these 4 from each object, by finding the point in the centre of the edge
            glm::vec3 ptOnEdgeOne = one.halfSize;
            glm::vec3 ptOnEdgeTwo = two.halfSize;
            for (unsigned i = 0; i < 3; i++) {
                if (i == axisOneIndex) ptOnEdgeOne[i] = 0;
                else if (glm::dot(one.getAxis(i), axis) > 0) ptOnEdgeOne[i] = -ptOnEdgeOne[i];

                if (i == axisTwoIndex) ptOnEdgeTwo[i] = 0;
                else if (glm::dot(two.getAxis(i), axis) > 0) ptOnEdgeTwo[i] = -ptOnEdgeTwo[i];
            }

            // Move them to world coords
            ptOnEdgeOne = one.transform * glm::vec4(ptOnEdgeOne, 1.0);
            ptOnEdgeTwo = two.transform * glm::vec4(ptOnEdgeTwo, 1.0);

            // Now we have a point and direction for the collidng edges, we need to find point
            // of closest approach of the two line segments
            glm::vec3 vertex = contactPoint(
                    ptOnEdgeOne, oneAxis, one.halfSize[axisOneIndex],
                    ptOnEdgeTwo, twoAxis, two.halfSize[axisTwoIndex],
                    bestSingleAxis > 2
                    );

            // Can finally fill in contact
            Contact* contact = data->contacts;
            contact->penetration = pen;
            contact->contactNormal = axis;
            contact->contactPoint = vertex;
            contact->setBodyData(one.body, two.body, data->friction, data->restitution);
            data->addContacts(1);
            VILLAIN_DEBUG("BoxVsBox contact normal {}", glm::to_string(contact->contactNormal));
            return 1;
        }

        return 0;
    }
}

#include "Collider.hpp"

#include "BoundingAABB.hpp"
#include "BoundingSphere.hpp"

namespace Villain {

    IntersectData Collider::intersect(const Collider& other) const {

        if (colliderType == ColliderType::SPHERE && other.getType() == ColliderType::SPHERE) {
            // Doesn't seem like a good way because of those casts
            BoundingSphere* self = (BoundingSphere*)this;
            return self->intersectBoundingSphere((BoundingSphere&)other);
        } else if (colliderType == ColliderType::AABB && other.getType() == ColliderType::AABB) {
            // Doesn't seem like a good way because of those casts
            BoundingAABB* self = (BoundingAABB*)this;
            return self->intersectAABB((BoundingAABB&)other);
        } else {
            // TODO: aabb/sphere or error because collider type is invalid
            return IntersectData(false, glm::vec3(1.f));
        }
    }
}

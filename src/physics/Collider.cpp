#include "Collider.hpp"

#include "BoundingAABB.hpp"
#include "BoundingSphere.hpp"
#include "Logger.hpp"

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
        } else if (colliderType == ColliderType::SPHERE && other.getType() == ColliderType::AABB) {
            BoundingSphere* self = (BoundingSphere*)this;
            return self->intersectAABB((BoundingAABB&)other);
        } else {
            Logger::Instance()->error("Collision type not supported");
            return IntersectData(false, glm::vec3(-1.f));
        }
    }
}

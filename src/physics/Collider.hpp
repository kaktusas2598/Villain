#ifndef __COLLIDER__
#define __COLLIDER__

#include "IntersectData.hpp"

namespace Villain {

    class DebugRenderer;

    // TODO: more collider types, mesh collider? Plane
    enum class ColliderType {
        SPHERE, AABB,
        TYPE_SIZE
    };

    // TODO: implement some kind of gc/ reference counter and inherit from it here
    class Collider {
        public:
            Collider(ColliderType type) : colliderType(type) {}

            IntersectData intersect(const Collider& other) const;
            virtual void transform(const glm::vec3& translation) {}
            virtual const glm::vec3& getCentre() const = 0;
            virtual void render(DebugRenderer* renderer) = 0;

            inline ColliderType getType() const { return colliderType; }
        private:
            ColliderType colliderType;
    };
}

#endif // __COLLIDER__

#ifndef __CAPSULE__
#define __CAPSULE__

#include "rendering/DebugRenderer.hpp"

#include <box2d/box2d.h>
#include <glm/glm.hpp>


// Binds 3 Box2D fixtures to create capsule like collision shape for entity sprites
class Capsule {
    public:
        void init(
                b2World* world,
                const glm::vec2& position,
                const glm::vec2& dimensions,
                float density,
                float friction,
                bool fixedRotation = false);

        // Ideally should be in separate class
        void drawDebug(Villain::DebugRenderer& debugRenderer);

        b2Body* getBody() { return body; }
        const b2Fixture* getFixture(int i) { return fixtures[i]; }
        const glm::vec2& getSize() { return size; }

    private:
        b2Body* body = nullptr;
        b2Fixture* fixtures[3];
        glm::vec2 size;

};

#endif // !__CAPSULE__

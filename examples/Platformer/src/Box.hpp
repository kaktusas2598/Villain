#ifndef __BOX__
#define __BOX__

#include <box2d/box2d.h>
#include <glm/glm.hpp>

class Box {
    public:
        Box();
        ~Box();

        void init(b2World* world, const glm::vec2& position, const glm::vec2& dimensions, const glm::vec4& col);

        const b2Body* getBody() { return body; }
        const b2Fixture* getFixture() { return fixture; }
        const glm::vec2& getSize() { return size; }
        const glm::vec4& getColor() { return color; }
    private:
        b2Body* body = nullptr;
        b2Fixture* fixture = nullptr;
        glm::vec2 size;
        glm::vec4 color;
};

#endif // __BOX__

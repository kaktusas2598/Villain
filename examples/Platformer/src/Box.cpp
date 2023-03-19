#include "Box.hpp"

Box::Box() {}

Box::~Box() {}

void Box::init(b2World* world, const glm::vec2& position, const glm::vec2& dimensions, const glm::vec4& col) {
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(position.x, position.y);
    body = world->CreateBody(&bodyDef);

    b2PolygonShape boxShape;
    boxShape.SetAsBox(dimensions.x / 2.0f, dimensions.y / 2.0f);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = & boxShape;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    fixture = body->CreateFixture(&fixtureDef);

    size = dimensions;
    color = col;
}

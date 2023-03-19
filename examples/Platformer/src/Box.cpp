#include "Box.hpp"

Box::Box() {}

Box::~Box() {}

void Box::init(
        b2World* world,
        const glm::vec2& position,
        const glm::vec2& dimensions,
        unsigned int texture,
        const glm::vec4& col,
        bool fixedRotation,
        glm::vec4 uv) {
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(position.x, position.y);
    bodyDef.fixedRotation = fixedRotation;
    body = world->CreateBody(&bodyDef);

    b2PolygonShape boxShape;
    boxShape.SetAsBox(dimensions.x / 2.0f, dimensions.y / 2.0f);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = & boxShape;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    fixture = body->CreateFixture(&fixtureDef);

    size = dimensions;
    textureID = texture;
    color = col;
    uvRect = uv;
}

void Box::draw(Villain::SpriteBatch& batch) {
    glm::vec4 destRect;
    // Need to subtract half size because box position is the centre
    destRect.x = body->GetPosition().x - size.x / 2.0f;
    destRect.y = body->GetPosition().y - size.y / 2.0f;
    destRect.z = size.x;
    destRect.w = size.y;
    batch.draw(destRect, uvRect, textureID, 0.0f, color, body->GetAngle());

}

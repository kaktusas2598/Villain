#include "Capsule.hpp"

void Capsule::init(
        b2World* world,
        const glm::vec2& position,
        const glm::vec2& dimensions,
        float density,
        float friction,
        bool fixedRotation) {
    size = dimensions;

    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(position.x, position.y);
    bodyDef.fixedRotation = fixedRotation;
    body = world->CreateBody(&bodyDef);

    // Create the box
    b2PolygonShape boxShape;
    boxShape.SetAsBox(dimensions.x / 2.0f, (dimensions.y - dimensions.x) / 2.0f);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &boxShape;
    fixtureDef.density = density;
    fixtureDef.friction = friction;
    fixtures[0] = body->CreateFixture(&fixtureDef);

    // Create the circles
    b2CircleShape circleShape;
    circleShape.m_radius = dimensions.x / 2.0f;

    b2FixtureDef circleDef;
    circleDef.shape = &circleShape;
    circleDef.density = density;
    circleDef.friction = friction;

    // Bottom circle
    circleShape.m_p.Set(0.0f, (-dimensions.y + dimensions.x) / 2.0f);
    fixtures[1] = body->CreateFixture(&circleDef);

    // Top circle
    circleShape.m_p.Set(0.0f, (dimensions.y - dimensions.x) / 2.0f);
    fixtures[2] = body->CreateFixture(&circleDef);



}

void Capsule::drawDebug(Villain::DebugRenderer& debugRenderer) {
    glm::vec4 color(1.0f);
    glm::vec4 destRect;
    destRect.x = body->GetPosition().x - size.x / 2.0f;
    destRect.y = body->GetPosition().y - (size.y - size.x) / 2.0f;
    destRect.z = size.x;
    destRect.w = size.y - size.x;
    debugRenderer.drawBox(destRect, 0.0f, glm::vec4(1.0f), body->GetAngle());

    // Bottom circle
    debugRenderer.drawCircle(glm::vec2(destRect.x + size.x / 2.0f, destRect.y), 0.0f, color, size.x / 2.0f);
    // Top circle
    debugRenderer.drawCircle(glm::vec2(destRect.x + size.x / 2.0f, destRect.y + destRect.w), 0.0f, color, size.x / 2.0f);
}

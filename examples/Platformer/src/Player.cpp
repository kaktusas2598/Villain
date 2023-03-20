#include "Player.hpp"
#include "InputManager.hpp"
#include <SDL2/SDL_keycode.h>


const float FRICTION = 0.3f;
const float DENSITY = 1.0f;

Player::Player() {

}
Player::~Player() {

}

void Player::init(
        b2World* world,
        const glm::vec2& position,
        const glm::vec2& drawDimensions,
        const glm::vec2& collisionDimensions,
        unsigned int texture,
        const glm::vec4& col) {
    textureID = texture;
    color = col;
    drawSize = drawDimensions;
    collisionShape.init(world, position, collisionDimensions * 2.f, DENSITY, FRICTION, true);
}

void Player::draw(Villain::SpriteBatch& batch) {
    //HACK:this is temporary
    //TODO: implement 2D animations properly
    glm::vec4 uvRect;
    int numColumns = 6;
    int numRows = 10;
    uvRect.x = 0 * 1.0f/numColumns;
    uvRect.y = (numRows - 1 - 1) * 1.0f/numRows;
    uvRect.z = 1.0f/numColumns;
    uvRect.w = 1.0f/numRows;

    glm::vec4 destRect;
    // Need to subtract half size because box position is the centre
    destRect.x = collisionShape.getBody()->GetPosition().x - drawSize.x / 2.0f;
    destRect.y = collisionShape.getBody()->GetPosition().y - collisionShape.getSize().y / 1.55f; // was 2.0f, probably because sprites are not centered in sheet
    destRect.z = drawSize.x;
    destRect.w = drawSize.y;
    batch.draw(destRect, uvRect, textureID, 0.0f, color, collisionShape.getBody()->GetAngle());
}

void Player::drawDebug(Villain::DebugRenderer& renderer) {
    collisionShape.drawDebug(renderer);
}

void Player::update() {
    Villain::InputManager* input = Villain::InputManager::Instance();
    b2Body* body = collisionShape.getBody();

    if (input->isKeyDown(SDLK_a)) {
        body->ApplyForceToCenter(b2Vec2(-100.0f, 0.0f), true);
    } else if (input->isKeyDown(SDLK_d)) {
        body->ApplyForceToCenter(b2Vec2(100.0f, 0.0f), true);
    } else {
        // Apply dampening
        body->SetLinearVelocity(b2Vec2(body->GetLinearVelocity().x * 0.95f, body->GetLinearVelocity().y));
    }

    float MAX_SPEED = 5.0f;
    if (body->GetLinearVelocity().x < -MAX_SPEED) {
        body->SetLinearVelocity(b2Vec2(-MAX_SPEED, body->GetLinearVelocity().y));
    } else if (body->GetLinearVelocity().x > MAX_SPEED) {
        body->SetLinearVelocity(b2Vec2(MAX_SPEED, body->GetLinearVelocity().y));
    }

    // Loop through all the contact points of player collision box
    for (b2ContactEdge* ce = body->GetContactList(); ce != nullptr; ce = ce->next) {
        b2Contact* c = ce->contact;
        if (c->IsTouching()) {
            b2WorldManifold manifold;
            c->GetWorldManifold(&manifold);
            // Check if points are below player
            bool below = false;
            for (int i = 0; i < b2_maxManifoldPoints; i++) {
                if (manifold.points[i].y < body->GetPosition().y - collisionShape.getSize().y / 2.0f + 0.01f) {
                    below = true;
                    break;
                }
            }
            if (below) {
                // now we can jump
                if (input->isKeyPressed(SDLK_SPACE)) {
                    body->ApplyLinearImpulse(b2Vec2(0.0f, 30.0f), b2Vec2(0.0f, 0.0f), true);
                    break;
                }
            }
        }
    }
}


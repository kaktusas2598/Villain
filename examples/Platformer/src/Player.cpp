#include "Player.hpp"
#include "InputManager.hpp"
#include <SDL2/SDL_keycode.h>

Player::Player() {

}
Player::~Player() {

}

void Player::init(b2World* world, const glm::vec2& position, const glm::vec2& dimensions, unsigned int texture, const glm::vec4& col) {
    //HACK:this is temporary
    //TODO: implement 2D animations properly
    glm::vec4 uvRect;
    int numColumns = 6;
    int numRows = 10;
    uvRect.x = 0 * 1.0f/numColumns;
    uvRect.y = (numRows - 1 - 1) * 1.0f/numRows;
    uvRect.z = 1.0f/numColumns;
    uvRect.w = 1.0f/numRows;

    collisionBox.init(world, position, dimensions * 2.f, texture, col, true, uvRect);
}

void Player::draw(Villain::SpriteBatch& batch) {
    collisionBox.draw(batch);
}

void Player::update() {
    Villain::InputManager* input = Villain::InputManager::Instance();
    b2Body* body = collisionBox.getBody();

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
                if (manifold.points[i].y < body->GetPosition().y - collisionBox.getSize().y / 2.0f + 0.01f) {
                    below = true;
                    break;
                }
            }
            if (below) {
                // now we can jump
                if (input->isKeyPressed(SDLK_SPACE)) {
                    body->ApplyLinearImpulse(b2Vec2(0.0f, 60.0f), b2Vec2(0.0f, 0.0f), true);
                    break;
                }
            }
        }
    }
}


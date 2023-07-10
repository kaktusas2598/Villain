#include "Player.hpp"
#include "Input.hpp"
#include "ResourceManager.hpp"
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
    Villain::Texture* t = Villain::ResourceManager::Instance()->getTexture("player");
    spriteSheet.init(t, glm::ivec2(6, 10));

    color = col;
    drawSize = drawDimensions;
    collisionShape.init(world, position, collisionDimensions * 2.f, DENSITY, FRICTION, true);
}

void Player::draw(Villain::SpriteBatch& batch) {

    float animationSpeed = 0.2;
    int frameIndex;
    int numFrames;

    glm::vec2 velocity;
    velocity.x = collisionShape.getBody()->GetLinearVelocity().x;
    velocity.y = collisionShape.getBody()->GetLinearVelocity().y;
    // Calculate animation
    if (onGround) {

        if (punching) {
            numFrames = 4;
            frameIndex = 12;
            animationSpeed *= 0.25f;
            if (moveState != PlayerMoveState::PUNCHING) {
                moveState = PlayerMoveState::PUNCHING;
                animationTime = 0;
            }
        } else if (abs(velocity.x) > 1.0f && (velocity.x > 0.0f && direction > 0) || (velocity.x < 0 && direction < 0)) {
            // running
            numFrames = 6;
            frameIndex = 48;
            animationSpeed = abs(velocity.x) * 0.025f;
            if (moveState != PlayerMoveState::RUNNING) {
                moveState = PlayerMoveState::RUNNING;
                animationTime = 0;
            }
        } else {
            // standing
            numFrames = 1;
            frameIndex = 48;
            moveState = PlayerMoveState::STANDING;
        }


        // jumping animation
    } else {
        if (punching) {
            numFrames = 4;
            frameIndex = 18;
            animationSpeed *= 0.25f;
            if (moveState != PlayerMoveState::PUNCHING) {
                moveState = PlayerMoveState::PUNCHING;
                animationTime = 0;
            }
        } else if (velocity.y <= 0.0f) {
            // falling
            numFrames = 1;
            frameIndex = 38;
            moveState = PlayerMoveState::JUMPING;
        } else {
            // rising
            numFrames = 1;
            frameIndex = 37;
            moveState = PlayerMoveState::JUMPING;
        }
    }

    animationTime += animationSpeed;

    // check for attack anim ending
    if (animationTime > numFrames) {
        punching = false;
    }

    frameIndex = frameIndex + (int)animationTime % numFrames;

    glm::vec4 uvRect = spriteSheet.getUVs(frameIndex);

    // flip uv coords if facing the other direction
    if (direction == -1) {
        uvRect.x += 1.0f/spriteSheet.dims.x;
        uvRect.z *= -1.0f;
    }

    glm::vec4 destRect;
    // Need to subtract half size because box position is the centre
    destRect.x = collisionShape.getBody()->GetPosition().x - drawSize.x / 2.0f;
    destRect.y = collisionShape.getBody()->GetPosition().y - collisionShape.getSize().y / 1.55f; // was 2.0f, probably because sprites are not centered in sheet
    destRect.z = drawSize.x;
    destRect.w = drawSize.y;
    batch.draw(destRect, uvRect, spriteSheet.texture->getID(), -0.5f, color, collisionShape.getBody()->GetAngle());
}

void Player::drawDebug(Villain::DebugRenderer& renderer) {
    collisionShape.drawDebug(renderer);
}

void Player::update() {
    Villain::Input* input = Villain::Input::Get();
    b2Body* body = collisionShape.getBody();

    if (input->isKeyPressed(SDLK_f)) {
        punching = true;
    }

    if (input->isKeyDown(SDLK_a)) {
        body->ApplyForceToCenter(b2Vec2(-100.0f, 0.0f), true);
        direction = -1;
    } else if (input->isKeyDown(SDLK_d)) {
        body->ApplyForceToCenter(b2Vec2(100.0f, 0.0f), true);
        direction = 1;
    } else {
        // Apply dampening
        body->SetLinearVelocity(b2Vec2(body->GetLinearVelocity().x * 0.95f, body->GetLinearVelocity().y));
    }

    float MAX_SPEED = 3.0f;
    if (body->GetLinearVelocity().x < -MAX_SPEED) {
        body->SetLinearVelocity(b2Vec2(-MAX_SPEED, body->GetLinearVelocity().y));
    } else if (body->GetLinearVelocity().x > MAX_SPEED) {
        body->SetLinearVelocity(b2Vec2(MAX_SPEED, body->GetLinearVelocity().y));
    }

    onGround = false;
    // Loop through all the contact points of player collision box
    for (b2ContactEdge* ce = body->GetContactList(); ce != nullptr; ce = ce->next) {
        b2Contact* c = ce->contact;
        if (c->IsTouching()) {
            b2WorldManifold manifold;
            c->GetWorldManifold(&manifold);
            // Check if points are below player
            bool below = false;
            for (int i = 0; i < b2_maxManifoldPoints; i++) {
                if (manifold.points[i].y < body->GetPosition().y - collisionShape.getSize().y / 2.0f + collisionShape.getSize().x / 2.0f + 0.01f) {
                    below = true;
                    break;
                }
            }
            if (below) {
                onGround = true;
                // now we can jump
                if (input->isKeyPressed(SDLK_SPACE)) {
                    body->ApplyLinearImpulse(b2Vec2(0.0f, 15.0f), b2Vec2(0.0f, 0.0f), true);
                    break;
                }
            }
        }
    }
}


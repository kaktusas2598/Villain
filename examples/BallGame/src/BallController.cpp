#include "BallController.hpp"
#include "glm/detail/qualifier.hpp"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"

void BallController::updateBalls(std::vector<Ball>& balls, float deltaTime, int maxX, int maxY) {
    const float FRICTION = 0.001f; // Friction coefficient
    if (grabbedBall != -1) {
        balls[grabbedBall].velocity = balls[grabbedBall].position - prevPos;
    }

    glm::vec2 gravity = getGravityAcc();

    for (int i = 0; i < balls.size(); i++) {
        Ball& ball = balls[i];
        if (i != grabbedBall) {
            ball.position += ball.velocity * deltaTime;
            // Apply friction
            glm::vec2 momentum = ball.velocity * ball.mass;
            if (momentum.x != 0 || momentum.y != 0) {
                if (FRICTION < glm::length(momentum)) {
                    ball.velocity -= deltaTime * FRICTION * glm::normalize(momentum) / ball.mass;
                } else {
                    ball.velocity = glm::vec2(0.0f);
                }
            }
            // Apply gravity
            ball.velocity += gravity * deltaTime;
        }
        // Check wall collisions
        if (ball.position.x < 0) {
            ball.position.x = 0;
            if (ball.velocity.x < 0) ball.velocity.x *= -1;
        } else if (ball.position.x + ball.radius * 2.0f >= maxX) {
            ball.position.x = maxX - ball.radius * 2.0f - 1;
            if (ball.velocity.x > 0) ball.velocity.x *= -1;
        }
        if (ball.position.y < 0) {
            ball.position.y = 0;
            if (ball.velocity.y < 0) ball.velocity.y *= -1;
        } else if (ball.position.y + ball.radius * 2.0f >= maxY) {
            ball.position.y = maxY - ball.radius * 2.0f - 1;
            if (ball.velocity.y > 0) ball.velocity.y *= -1;
        }
        // Check collisions
        for (int j = i + 1; j < balls.size(); j++) {
            checkCollision(ball, balls[j]);
        }
    }

    // Update grabbedBall
    if (grabbedBall != -1) {
        balls[grabbedBall].velocity = balls[grabbedBall].position - prevPos;
        prevPos = balls[grabbedBall].position;
    }
}

void BallController::onMouseDown(std::vector<Ball>& balls, float mouseX, float mouseY) {
    for (int i = 0; i < balls.size(); i++) {
       if (isMouseOnBall(balls[i], mouseX, mouseY))  {
           grabbedBall = i; // NOTE: This will become INVALID if vector of balls gets sorted
           grabOffset = glm::vec2(mouseX, mouseY) - balls[i].position;
           prevPos = balls[i].position;
           balls[i].velocity = glm::vec2(0.0f);
       }
    }
}

void BallController::onMouseUp(std::vector<Ball>& balls) {
    if (grabbedBall != -1) {
        // throw the ball
        balls[grabbedBall].velocity = balls[grabbedBall].position - prevPos;
        grabbedBall = -1;
    }
}

void BallController::onMouseMove(std::vector<Ball>& balls, float mouseX, float mouseY) {
    if (grabbedBall != -1) {
        balls[grabbedBall].position = glm::vec2(mouseX, mouseY) - grabOffset;
    }
}

void BallController::checkCollision(Ball& b1, Ball& b2) {
    glm::vec2 distVec = (b2.position + b2.radius) - (b1.position + b1.radius);
    glm::vec2 distDir = glm::normalize(distVec);
    float dist = glm::length(distVec);
    float totalRadius = b1.radius + b2.radius;

    float collisionDepth = totalRadius - dist;

    if (collisionDepth > 0) {
        // Push away ball with less mass
        if (b1.mass < b2.mass) {
        } else {
            b2.position -= distDir * collisionDepth;
        }

        //calculate deflection
        float aci = glm::dot(b1.velocity, distDir) / b2.mass;
        float bci = glm::dot(b2.velocity, distDir) / b1.mass;

        float massRatio = b1.mass / b2.mass;

        b1.velocity += (bci - aci) * distDir * (1.0f / massRatio);
        b2.velocity += (bci - aci) * distDir * massRatio;
    }
}

bool BallController::isMouseOnBall(Ball&b, float mouseX, float mouseY) {
    return (mouseX >= b.position.x && mouseX < b.position.x + b.radius * 2.0f &&
            mouseY >= b.position.y && mouseY < b.position.y + b.radius * 2.0f);
}

glm::vec2 BallController::getGravityAcc() {
    const float GRAVITY_FORCE = 0.1f;
    glm::vec2 gravity;

    switch (gravityDirection) {
        case GravityDirection::DOWN:
            gravity = glm::vec2(0.0f, -GRAVITY_FORCE);
            break;
        case GravityDirection::LEFT:
            gravity = glm::vec2(-GRAVITY_FORCE, 0.0f);
            break;
        case GravityDirection::RIGHT:
            gravity = glm::vec2(GRAVITY_FORCE, 0.0f);
            break;
        case GravityDirection::UP:
            gravity = glm::vec2(0.0f, GRAVITY_FORCE);
            break;
        default:
            gravity = glm::vec2(0.0f);
            break;
    }
    return gravity;
}

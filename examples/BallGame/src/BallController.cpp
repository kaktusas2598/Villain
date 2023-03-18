#include "BallController.hpp"
#include "Grid.hpp"
#include "glm/detail/qualifier.hpp"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"

void BallController::updateBalls(std::vector<Ball>& balls, Grid* grid, float deltaTime, int maxX, int maxY) {
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
        if (ball.position.x < ball.radius) {
            ball.position.x = ball.radius;
            if (ball.velocity.x < 0) ball.velocity.x *= -1;
        } else if (ball.position.x + ball.radius >= maxX) {
            ball.position.x = maxX - ball.radius - 1;
            if (ball.velocity.x > 0) ball.velocity.x *= -1;
        }
        if (ball.position.y < ball.radius) {
            ball.position.y = ball.radius;
            if (ball.velocity.y < 0) ball.velocity.y *= -1;
        } else if (ball.position.y + ball.radius >= maxY) {
            ball.position.y = maxY - ball.radius - 1;
            if (ball.velocity.y > 0) ball.velocity.y *= -1;
        }

        Cell* newCell = grid->getCell(ball.position);
        if (newCell != ball.ownerCell) {
            // Shift the ball
            grid->removeBallFromCell(&balls[i]);
            grid->addBall(&balls[i], newCell);
        }

        // Check collisions
        //for (int j = i + 1; j < balls.size(); j++) {
            //checkCollision(grid, ball, balls[j]);
        //}
    }
    // Check collisions
    updateCollision(grid);

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

void BallController::updateCollision(Grid* grid) {
    for (int i = 0; i < grid->cells.size(); i++) {
        int x = i % grid->numXCells;
        int y = i / grid->numXCells;

        Cell& cell = grid->cells[i];
        for(int j = 0; j < cell.balls.size(); j++) {
            Ball* ball = cell.balls[j];
            // Update collision within same cell as ball resides
            checkCollision(ball, cell.balls, j + 1);
            // Neighbour cell collisions
            if (x > 0) {
                if (y > 0) {
                    // Top left cell
                    checkCollision(ball, grid->getCell(x - 1, y - 1)->balls, 0);
                }
                if (y < grid->numYCells - 1) {
                    // Bottom left
                    checkCollision(ball, grid->getCell(x - 1, y + 1)->balls, 0);
                }
            }
            // Up cell
            if (y > 0) {
                checkCollision(ball, grid->getCell(x, y - 1)->balls, 0);
            }
        }
    }
}

void BallController::checkCollision(Ball* ball, std::vector<Ball*>& ballsToCheck, int startingIndex) {
    for (int i = startingIndex; i < ballsToCheck.size(); i++) {
        checkCollision(*ball, *ballsToCheck[i]);
    }
}

void BallController::checkCollision(Ball& b1, Ball& b2) {
    // circle - circle collision
    glm::vec2 distVec = b2.position - b1.position;
    glm::vec2 distDir = glm::normalize(distVec);
    float dist = glm::length(distVec);
    float totalRadius = b1.radius + b2.radius;

    float collisionDepth = totalRadius - dist;

    if (collisionDepth > 0) {
        // Push away ball with less mass
        //if (b1.mass < b2.mass) {
            //b1.position -= distDir * collisionDepth;
        //} else {
            //b2.position += distDir * collisionDepth;
        //}
        // Push the balls based on ratio of mass
        float massRatio = b1.mass / b2.mass;
        b1.position -= distDir * collisionDepth * massRatio * 0.5f;
        b2.position += distDir * collisionDepth * massRatio * 0.5f;

        //calculate deflection - http://stackoverflow.com/a/345863
        // Elastic ball collision
        float aci = glm::dot(b1.velocity, distDir) / b2.mass;
        float bci = glm::dot(b2.velocity, distDir) / b1.mass;

        float acf = (aci * (b1.mass - b2.mass) + 2 * b2.mass * bci) / (b1.mass + b2.mass);
        float bcf = (bci * (b2.mass - b1.mass) + 2 * b1.mass * aci) / (b1.mass + b2.mass);

        b1.velocity += (acf - aci) * distDir;
        b2.velocity += (bcf - bci) * distDir;
    }
}

bool BallController::isMouseOnBall(Ball&b, float mouseX, float mouseY) {
    return (mouseX >= b.position.x - b.radius && mouseX < b.position.x + b.radius &&
            mouseY >= b.position.y - b.radius && mouseY < b.position.y + b.radius);
}

glm::vec2 BallController::getGravityAcc() {
    const float GRAVITY_FORCE = 0.75f; // 0.1f
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

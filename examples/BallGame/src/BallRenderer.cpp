#include "BallRenderer.hpp"

void BallRenderer::renderBall(Villain::SpriteBatch& batch, Ball& ball) {
    glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);
    glm::vec4 destRect(ball.position.x - ball.radius, ball.position.y - ball.radius,
            ball.radius * 2.0f, ball.radius * 2.0f);
    batch.draw(destRect, uvRect, ball.textureID, 0.0f, ball.color);
}

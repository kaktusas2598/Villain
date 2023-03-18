#include "BallRenderer.hpp"

#include "Engine.hpp"
#include "ResourceManager.hpp"

void BallRenderer::renderBalls(
        Villain::SpriteBatch& batch,
        const std::vector<Ball>& balls,
        const glm::mat4& viewMatrix) {

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    // Lazy initialisation - we only load shader once it is needed
    if (shader == nullptr) {
        shader = Villain::ResourceManager::Instance()->loadShader("assets/shaders/spriteBatch.vert", "assets/shaders/spriteBatch.frag", "balls");
    }

    shader->bind();
    shader->setUniform1i("spriteTexture", 0);
    shader->setUniformMat4f("projection", glm::mat4(1.0f));
    shader->setUniformMat4f("view", viewMatrix);

    batch.begin();

    for (auto& ball: balls) {
        glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);
        glm::vec4 destRect(ball.position.x - ball.radius, ball.position.y - ball.radius,
        ball.radius * 2.0f, ball.radius * 2.0f);
        batch.draw(destRect, uvRect, ball.textureID, 0.0f, ball.color);
    }


    batch.end();
    batch.renderBatch();

    shader->unbind();
}

void MomentumBallRenderer::renderBalls(
        Villain::SpriteBatch& batch,
        const std::vector<Ball>& balls,
        const glm::mat4& viewMatrix) {

    glClearColor(0.0f, 0.1f, 0.5f, 1.0f);
    // Lazy initialisation - we only load shader once it is needed
    if (shader == nullptr) {
        shader = Villain::ResourceManager::Instance()->loadShader("assets/shaders/spriteBatch.vert", "assets/shaders/spriteBatch.frag", "balls");
    }

    shader->bind();
    shader->setUniform1i("spriteTexture", 0);
    shader->setUniformMat4f("projection", glm::mat4(1.0f));
    shader->setUniformMat4f("view", viewMatrix);

    batch.begin();

    for (auto& ball: balls) {
        glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);
        glm::vec4 destRect(ball.position.x - ball.radius, ball.position.y - ball.radius,
        ball.radius * 2.0f, ball.radius * 2.0f);
        glm::vec4 color;
        // Color val based on ball's momentum
        float colorVal = glm::clamp(glm::length(ball.velocity) * ball.mass, 0.0f, 1.0f);
        color.r = colorVal;
        color.g = colorVal;
        color.b = colorVal;
        color.a = colorVal;
        batch.draw(destRect, uvRect, ball.textureID, 0.0f, color);
    }


    batch.end();
    batch.renderBatch();

    shader->unbind();
}


void VelocityBallRenderer::renderBalls(Villain::SpriteBatch& batch, const std::vector<Ball>& balls, const glm::mat4& viewMatrix) {
    glClearColor(0.0f, 0.1f, 0.0f, 1.0f);
    // Lazy initialisation - we only load shader once it is needed
    if (shader == nullptr) {
        shader = Villain::ResourceManager::Instance()->loadShader("assets/shaders/spriteBatch.vert", "assets/shaders/spriteBatch.frag", "balls");
    }

    shader->bind();
    shader->setUniform1i("spriteTexture", 0);
    shader->setUniformMat4f("projection", glm::mat4(1.0f));
    shader->setUniformMat4f("view", viewMatrix);

    batch.begin();

    for (auto& ball: balls) {
        glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);
        glm::vec4 destRect(ball.position.x - ball.radius, ball.position.y - ball.radius,
        ball.radius * 2.0f, ball.radius * 2.0f);
        glm::vec4 color;
        // Color val based on ball's momentum
        float mult = 100.0f;
        // Change color based on velocity along X axis
        float colorVal = glm::clamp(ball.velocity.x * mult, 0.0f, 1.0f);
        color.r = 0.5f;
        color.g = ball.position.x / Villain::Engine::getScreenWidth();
        color.b = ball.position.y / Villain::Engine::getScreenHeight();
        color.a = colorVal;
        batch.draw(destRect, uvRect, ball.textureID, 0.0f, color);
    }


    batch.end();
    batch.renderBatch();

    shader->unbind();

}

void TrippyBallRenderer::renderBalls(Villain::SpriteBatch& batch, const std::vector<Ball>& balls, const glm::mat4& viewMatrix) {
    //TODO:
    glClearColor(0.1f, 0.0f, 0.0f, 1.0f);
    // Lazy initialisation - we only load shader once it is needed
    if (shader == nullptr) {
        shader = Villain::ResourceManager::Instance()->loadShader("assets/shaders/spriteBatch.vert", "assets/shaders/spriteBatch.frag", "balls");
    }

    shader->bind();
    shader->setUniform1i("spriteTexture", 0);
    shader->setUniformMat4f("projection", glm::mat4(1.0f));
    shader->setUniformMat4f("view", viewMatrix);

    batch.begin();

    float TIME_SPEED = 0.01f;
    float DIVISOR = 4.0f; // <<< for more spiral arms
    float SPIRAL_INTENSITY = 10.0f;

    time += TIME_SPEED;

    for (auto& ball: balls) {
        glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);
        glm::vec4 destRect(ball.position.x - ball.radius, ball.position.y - ball.radius,
        ball.radius * 2.0f, ball.radius * 2.0f);
        glm::vec4 color;

        //vector from central point in screen
        glm::vec2 centerVec = ball.position - glm::vec2(Villain::Engine::getScreenWidth() / 2, Villain::Engine::getScreenHeight() / 2);
        float centerDist = glm::length(centerVec);

        // angle from horizontal
        float angle = atan2(centerVec.x, centerVec.y) / (M_PI / DIVISOR);
        // move spiral with time
        angle -= time;
        // add the spiral
        angle += (centerDist / Villain::Engine::getScreenWidth()) * SPIRAL_INTENSITY;

        color.r = angle;
        color.g = angle * cos(time);
        color.b = angle * sin(time);
        color.a = glm::clamp(1.0f - (centerDist / (Villain::Engine::getScreenWidth() / 2.f)), 0.0f, 1.0f);
        batch.draw(destRect, uvRect, ball.textureID, 0.0f, color);
    }


    batch.end();
    batch.renderBatch();

    shader->unbind();

}

#include "Ball.hpp"

Ball::Ball(float radius, float mass, const glm::vec2& pos, const glm::vec2& vel,
            unsigned int texture, const glm::vec4& color) {
    this->radius = radius;
    this->mass = mass;
    this->position = pos;
    this->velocity = vel;
    this->textureID = texture;
    this->color = color;
}


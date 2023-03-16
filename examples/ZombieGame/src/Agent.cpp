#include "Agent.hpp"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include <algorithm>

#include <iostream>

Agent::Agent() {}

Agent::~Agent() {}

bool Agent::collideWithLevel(Villain::Level& level) {
    float spriteSize = scale * frameSize;

    // TODO: check all collidable layers against agent not just 1st one
    // Check four corners around sprite
    std::vector<glm::vec2> collideTilePositions;
    checkTilePosition(level, collideTilePositions, position.x, position.y);
    checkTilePosition(level, collideTilePositions, position.x + spriteSize, position.y);
    checkTilePosition(level, collideTilePositions, position.x, position.y + spriteSize);
    checkTilePosition(level, collideTilePositions, position.x + spriteSize, position.y + spriteSize);

    if(collideTilePositions.size() == 0)
        return false;

    std::vector<Villain::TileLayer*> layers = level.getCollidableLayers();
    float tileSize = layers[0]->getTileSize() * layers[0]->getScale();
    for (int i = 0; i < collideTilePositions.size(); i++) {
       collideWithTile(collideTilePositions[i], tileSize);
    }

    return true;
}


// Circular collision 2D
bool Agent::collideWithAgent(Agent* agent) {
    const float spriteRadius = scale * frameSize / 2;
    glm::vec2 centrePositionA = glm::vec2(position) + glm::vec2(spriteRadius);
    glm::vec2 centrePositionB = glm::vec2(agent->getPosition()) + glm::vec2(spriteRadius);
    const float minDistance = spriteRadius * 2;

    glm::vec2 distVec = centrePositionA - centrePositionB;

    float collisionDepth = minDistance - glm::length(distVec);
    // Agent collision!
    if (collisionDepth > 0) {
        glm::vec2 collisionDepVec = glm::normalize(distVec) * collisionDepth;
        position.x += collisionDepVec.x / 2.0f;
        position.y += collisionDepVec.y / 2.0f;
        agent->position.x -= collisionDepVec.x / 2.0f;
        agent->position.y -= collisionDepVec.y / 2.0f;
        return true;
    }

    return false;
}

void Agent::draw(Villain::SpriteBatch& batch) {
    glm::vec4 destRect;
    destRect.x = position.x;
    destRect.y = position.y;
    int size = floor(frameSize);
    destRect.z = scale * size;
    destRect.w = scale * size;
    // TODO:Utilise color, setup animations
    glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);
    batch.draw(destRect, uvRect, texture->getID(), position.z, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), direction);
    //batch.draw(destRect, 0, 0, size, size, texture, position.z, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

}

bool Agent::applyDamage(float dmg) {
    health -= dmg;
    if (health <= 0) {
        return true;
    }
    return false;
}

void Agent::checkTilePosition(Villain::Level& level, std::vector<glm::vec2>& collideTilePositions, float x, float y) {
    std::vector<Villain::TileLayer*> layers = level.getCollidableLayers();

    float tileSize = layers[0]->getTileSize() * layers[0]->getScale();
    auto tileIds = layers[0]->getTileIDs();

    glm::vec2 cornerPos = glm::vec2(floor(x / tileSize), floor(y / tileSize));

    if (cornerPos.x < layers[0]->getMapWidth() || cornerPos.x >= 0
            || cornerPos.y < layers[0]->getMapHeight() || cornerPos.y >= 0) {
        if (tileIds[cornerPos.y][cornerPos.x] != 0) {
            // Use center position of tile for collision check
            collideTilePositions.push_back(cornerPos * tileSize + glm::vec2(tileSize / 2.0f));
        }
    }
}


// AABB Collision, circle/tile
void Agent::collideWithTile(glm::vec2 tilePosition, float tileSize) {
    const float spriteRadius = scale * frameSize / 2;
    const float tileRadius = tileSize / 2;
    const float minDistance = spriteRadius + tileRadius;

    glm::vec2 spriteCentre = glm::vec2(position) + glm::vec2(spriteRadius);
    glm::vec2 distance = spriteCentre - tilePosition;

    // Calculate collision penetration depth
    float xDepth = minDistance - abs(distance.x);
    float yDepth = minDistance - abs(distance.y);

    // Definitely collision!
    if (xDepth > 0 && yDepth > 0) {
        if (std::max(xDepth, 0.0f) < std::max(yDepth, 0.0f)) {
            if (distance.x < 0)
                position.x -= xDepth;
            else
                position.x += xDepth;
        } else {
            if (distance.y < 0)
                position.y -= yDepth;
            else
                position.y += yDepth;

        }
    }
}

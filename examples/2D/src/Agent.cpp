#include "Agent.hpp"
#include "glm/fwd.hpp"
#include <algorithm>

#include <iostream>

Agent::Agent()
{
}

Agent::~Agent()
{
}

void Agent::collideWithLevel(Villain::Level& level) {
    // TODO: check all collidable layers against agent not just 1st one
    std::vector<Villain::TileLayer*> layers = level.getCollidableLayers();
    //for (size_t i = 0; i < layers.size(); i++) {
        //auto tileIds = layers[i]->getTileIDs();
    //}

    std::vector<glm::vec2> collideTilePositions;
    float spriteSize = 48.0f; //TODO: fix this hardcoded value, same as in draw()

    // Check four corners around sprite
    float tileSize = layers[0]->getTileSize() * layers[0]->getScale();
    checkTilePosition(level, collideTilePositions, position.x, position.y);
    checkTilePosition(level, collideTilePositions, position.x + spriteSize, position.y);
    checkTilePosition(level, collideTilePositions, position.x, position.y + spriteSize);
    checkTilePosition(level, collideTilePositions, position.x + spriteSize, position.y + spriteSize);

    for (int i = 0; i < collideTilePositions.size(); i++) {
       collideWithTile(collideTilePositions[i], tileSize);
    }
}

void Agent::draw(Villain::SpriteBatch& batch) {
    glm::vec4 destRect;
    destRect.x = position.x;
    destRect.y = position.y;
    //FIXME: pass in frame size instread of hardcode
    int size = 48;
    destRect.z = size;
    destRect.w = size;
    //spriteBatch->draw();
    //Texture* texture = ResourceManager::Instance()->getTexture(tileset.name);
    // TODO:Utilise color, setup animations
    //batch->draw(destRect, frame, row, frameWidth, frameHeight, texture, depth, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    batch.draw(destRect, 0, 0, size, size, texture, position.z, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

}


void Agent::checkTilePosition(Villain::Level& level, std::vector<glm::vec2>& collideTilePositions, float x, float y) {
    std::vector<Villain::TileLayer*> layers = level.getCollidableLayers();

    float tileSize = layers[0]->getTileSize() * layers[0]->getScale();
    auto tileIds = layers[0]->getTileIDs();

    glm::vec2 cornerPos = glm::vec2(floor(x / tileSize), floor(y / tileSize));
    std::cout << "Player Tile Position: " << cornerPos.x << ", " << cornerPos.y << "\n";

    if (cornerPos.x < layers[0]->getMapWidth() && cornerPos.x >= 0
        && cornerPos.y < layers[0]->getMapHeight() && cornerPos.y >= 0) {
        if (tileIds[cornerPos.x][cornerPos.y] != 0) {
            std::cout << "Player Tile Collision!\n";
            // Use center position of tile for collision check
            collideTilePositions.push_back(cornerPos * tileSize + glm::vec2(tileSize / 2.0f));
        }
    }
}


// AABB Collision, circle/tile
void Agent::collideWithTile(glm::vec2 tilePosition, float tileSize) {
    const float spriteRadius = 48.0f / 2; //TODO: fix this hardcoded value, same as in draw()
    const float tileRadius = tileSize / 2;
    const float minDistance = spriteRadius + tileRadius;

    glm::vec2 spriteCentre = glm::vec2(position) + glm::vec2(spriteRadius);
    glm::vec2 distance = spriteCentre - tilePosition;

    // Calculate collision penetration depth
    float xDepth = minDistance - abs(distance.x);
    float yDepth = minDistance - abs(distance.y);
    std::cout << "Penetration depth(x, y): "<< xDepth << ", " << yDepth << "\n";

    // Definitely collision!
    if (xDepth > 0 || yDepth > 0) {
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

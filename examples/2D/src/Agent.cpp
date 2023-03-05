#include "Agent.hpp"

Agent::Agent()
{
}

Agent::~Agent()
{
}

void Agent::draw(Villain::SpriteBatch& batch) {
    glm::vec4 destRect(1,1,1,1);
    destRect.x = position.x;
    destRect.y = position.y;
    //FIXME: pass in frame size instread of hardcode
    int size = 32;
    destRect.z = size;
    destRect.w = size;
    //spriteBatch->draw();
    //Texture* texture = ResourceManager::Instance()->getTexture(tileset.name);
    // TODO:Utilise color, spacing and margin can be implemented if needed
    //batch->draw(destRect, frame, row, frameWidth, frameHeight, texture, depth, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    batch.draw(destRect, 0, 0, size, size, texture, position.z, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

}

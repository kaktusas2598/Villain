#include "ResourceManager.hpp"

namespace Villain {

    ResourceManager* ResourceManager::sInstance = nullptr;

    void ResourceManager::clearTextureMap() {
        textureMap.clear();
    }

    void ResourceManager::clearTexture(std::string id) {
        delete textureMap[id];
        textureMap.erase(id);
    }

    void ResourceManager::clearShaderMap() {
        shaderMap.clear();
    }

    void ResourceManager::clearShader(std::string id) {
        delete shaderMap[id];
        shaderMap.erase(id);
    }
}

#include "ResourceManager.hpp"

#include "FileUtils.hpp"

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

    Texture* ResourceManager::loadTexture(std::string fileName, std::string id, GLint wrappingMode, bool gammaCorrected) {
        if (textureMap.find(id) != textureMap.end())
            return textureMap[id];

        Texture* texture = nullptr;
        for (const std::string& directory : searchDirectories) {
            std::string fullPath = directory + fileName;
            if (FileUtils::fileExists(fullPath)) {
                TextureConstructionInfo texInfo = TextureConstructionInfo();
                texInfo.WrappingMode = wrappingMode;
                texInfo.SRGB = gammaCorrected;
                texture = new Texture(fullPath, texInfo);
                break;
            }
        }

        if (texture) {
            textureMap[id] = texture;
            return texture;
        } else {
            // Resource not found
            return nullptr;
        }
    }

    Shader* ResourceManager::loadShader(const std::string& fileName, std::string id) {
        if (shaderMap.find(id) != shaderMap.end())
            return shaderMap[id];

        Shader* shader = nullptr;
        for (const std::string& directory : searchDirectories) {
            std::string fullPath = directory + fileName;
            if (FileUtils::fileExists(fullPath)) {
                shader = new Shader(fullPath);
                break;
            }
        }

        if (shader) {
            shaderMap[id] = shader;
            return shader;
        } else {
            // Resource not found
            return nullptr;
        }
    }

    Shader* ResourceManager::loadShader(const std::string& vertPath, const std::string& fragPath, std::string id) {
        if (shaderMap.find(id) != shaderMap.end())
            return shaderMap[id];

        Shader* shader = new Shader(vertPath, fragPath);
        shaderMap[id] = shader;
        return shaderMap[id];
    }

}

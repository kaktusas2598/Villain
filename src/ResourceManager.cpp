#include "ResourceManager.hpp"

#include "FileUtils.hpp"
#include "Logger.hpp"
#include "rendering/PBRMaterial.hpp"

namespace Villain {

    ResourceManager* ResourceManager::sInstance = nullptr;

    Material* ResourceManager::getMaterial(const std::string& id) {
        if (materialMap.find(id) != materialMap.end()) {
            return materialMap[id];
        } else {
            VILLAIN_WARN("Material {} not found", id);
            return nullptr;
        }
    }

    Model* ResourceManager::getModel(const std::string& id) {
        if (modelMap.find(id) != modelMap.end()) {
            return modelMap[id];
        } else {
            VILLAIN_WARN("Model {} not found", id);
            return nullptr;
        }
    }


    Shader* ResourceManager::getShader(const std::string& id) {
        if (shaderMap.find(id) != shaderMap.end()) {
            return shaderMap[id];
        } else {
            VILLAIN_WARN("Shader {} not found", id);
            return nullptr;
        }
    }

    Texture* ResourceManager::getTexture(const std::string& id) {
        if (textureMap.find(id) != textureMap.end()) {
            return textureMap[id];
        } else {
            VILLAIN_WARN("Texture {} not found", id);
            return nullptr;
        }
    }

    AudioBuffer* ResourceManager::getAudio(const std::string& id) {
        if (audioMap.find(id) != audioMap.end()) {
            return audioMap[id];
        } else {
            VILLAIN_WARN("Audio {} not found", id);
            return nullptr;
        }
    }

    void ResourceManager::clearAudioMap() {
        audioMap.clear();
    }

    void ResourceManager::clearAudio(std::string id) {
        delete audioMap[id];
        audioMap.erase(id);
    }

    void ResourceManager::clearMaterialMap() {
        materialMap.clear();
    }

    void ResourceManager::clearMaterial(std::string id) {
        delete materialMap[id];
        materialMap.erase(id);
    }

    void ResourceManager::clearModelMap() {
        modelMap.clear();
    }

    void ResourceManager::clearModel(std::string id) {
        delete modelMap[id];
        modelMap.erase(id);
    }

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

    void ResourceManager::addMaterial(Material* material) {
        // Only add to map if material by same name doesn't exist
        if (materialMap.find(material->getName()) == materialMap.end())
            materialMap[material->getName()] = material;;
    }

    Material* ResourceManager::loadPBRMaterial(const std::string& id,
                    const std::string& albedoMapFile,
                    const std::string& normalMapFile,
                    const std::string& roughnessMapFile,
                    const std::string& metallicMapFile,
                    const std::string& aoMapFile) {
        if (materialMap.find(id) == materialMap.end()) {
            Material* mat = nullptr;
            mat = new PBRMaterial(id,
                    loadTexture(albedoMapFile, "albedo", GL_CLAMP_TO_EDGE, false),
                    loadTexture(normalMapFile, "normal", GL_CLAMP_TO_EDGE, false),
                    loadTexture(metallicMapFile, "metallic", GL_CLAMP_TO_EDGE, false),
                    loadTexture(roughnessMapFile, "roughness", GL_CLAMP_TO_EDGE, false),
                    loadTexture(aoMapFile, "ao", GL_CLAMP_TO_EDGE, false)
            );
            materialMap[id] = mat;
            return mat;
        }
        return materialMap[id];
    }

    Model* ResourceManager::loadModel(const std::string& fileName, std::string id) {
        if (modelMap.find(id) != modelMap.end())
            return modelMap[id];

        Model* model = nullptr;
        for (const std::string& directory : searchDirectories) {
            std::string fullPath = directory + fileName;
            if (FileUtils::fileExists(fullPath)) {
                model = new Model(fullPath.c_str());
                break;
            }
        }

        if (model) {
            modelMap[id] = model;
            return model;
        } else {
            // Resource not found
            VILLAIN_ERROR("Model {} not found!", fileName);
            return nullptr;
        }
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
            VILLAIN_ERROR("Texture {} not found!", fileName);
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
            VILLAIN_ERROR("Shader {} not found!", fileName);
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

    AudioBuffer* ResourceManager::loadAudio(const std::string& fileName, std::string id) {
        if (audioMap.find(id) != audioMap.end())
            return audioMap[id];

        AudioBuffer* audio = nullptr;
        for (const std::string& directory : searchDirectories) {
            std::string fullPath = directory + fileName;
            if (FileUtils::fileExists(fullPath)) {
                audio = new AudioBuffer(fullPath.c_str());
                break;
            }
        }

        if (audio) {
            audioMap[id] = audio;
            return audio;
        } else {
            // Resource not found
            VILLAIN_ERROR("Audio file {} not found!", fileName);
            return nullptr;
        }
    }
}

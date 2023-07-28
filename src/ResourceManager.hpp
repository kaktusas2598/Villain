#pragma once

#include <string>
#include <map>
#include "rendering/Texture.hpp"
#include "rendering/Shader.hpp"

namespace Villain {

    /*
     * Global Resource Management system
     */
    class ResourceManager {
        public:

            Texture* loadTexture(
                    std::string fileName,
                    std::string id,
                    GLint wrappingMode = GL_CLAMP_TO_EDGE,
                    bool gammaCorrected = true);

            Shader* loadShader(const std::string& fileName, std::string id);
            Shader* loadShader(const std::string& vertPath, const std::string& fragPath, std::string id);
            //Shader* loadShader(const std::string& vertPath, const std::string& fragPath, const std::string& geoPath, std::string id) {}
            //TODO: Models, audio, fonts
            //TODO: Hot reloading
            //TODO: Search for embedded resources as well

            void setSearchDirectories(const std::vector<std::string>& directories) { searchDirectories = directories; }
            void addSearchDirectory(const std::string& directory) { searchDirectories.push_back(directory); }

            Texture* getTexture(std::string id) {
                if (textureMap.find(id) != textureMap.end())
                    return textureMap[id];
                else
                    return nullptr;
            }

            Shader* getShader(std::string id) {
                if (shaderMap.find(id) != shaderMap.end())
                    return shaderMap[id];
                else
                    return nullptr;
            }

            static ResourceManager* Instance() {
                if(sInstance == 0) {
                    sInstance = new ResourceManager();
                    return sInstance;
                }
                return sInstance;
            }

            std::map<std::string, Texture*>& getTextureMap() { return textureMap; }
            void clearTextureMap();
            void clearTexture(std::string id);

            std::map<std::string, Shader*>& getShaderMap() { return shaderMap; }
            void clearShaderMap();
            void clearShader(std::string id);

        private:
            ResourceManager() {}

            std::map<std::string, Texture*> textureMap;
            std::map<std::string, Shader*> shaderMap;

            // Resource search directories
            std::vector<std::string> searchDirectories = {
                "", // For absolute paths
                "./",
                "../",
                "assets",
                "assets/shaders/",
                "assets/textures/",
                "resources",
                "resources/shaders/",
                "resources/textures/",
            };

            static ResourceManager* sInstance;
    };
}

#ifndef __ResourceManager__
#define __ResourceManager__

#include <string>
#include <map>
#include "rendering/Texture.hpp"
#include "rendering/Shader.hpp"

namespace Villain {

    /*
     * Global Resource Manager
     */
    class ResourceManager {
        public:

            Texture* loadTexture(std::string fileName, std::string id, GLint wrappingMode = GL_CLAMP_TO_EDGE, bool gammaCorrected = true) {
                if (textureMap.find(id) != textureMap.end())
                    return textureMap[id];
                TextureConstructionInfo texInfo = TextureConstructionInfo();
                texInfo.WrappingMode = wrappingMode;
                texInfo.SRGB = gammaCorrected;
                Texture* texture = new Texture(fileName, texInfo);
                //Texture* texture = new Texture(fileName, wrappingMode, gammaCorrected);
                textureMap[id] = texture;
                return textureMap[id];
            }

            Shader* loadShader(const std::string& fileName, std::string id) {
                if (shaderMap.find(id) != shaderMap.end())
                    return shaderMap[id];
                Shader* shader = new Shader(fileName);
                shaderMap[id] = shader;
                return shaderMap[id];

            }

            Shader* loadShader(const std::string& vertPath, const std::string& fragPath, std::string id) {
                if (shaderMap.find(id) != shaderMap.end())
                    return shaderMap[id];
                Shader* shader = new Shader(vertPath, fragPath);
                shaderMap[id] = shader;
                return shaderMap[id];
            }

            //Shader* loadShader(const std::string& vertPath, const std::string& fragPath, const std::string& geoPath, std::string id) {}

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

            static ResourceManager* sInstance;
    };
}

#endif // __ResourceManager__

#pragma once

#include <string>
#include <map>

#include "audio/AudioBuffer.hpp"
#include "rendering/HDRMap.hpp"
#include "rendering/Model.hpp"
#include "rendering/Shader.hpp"
#include "rendering/Texture.hpp"

namespace Villain {

    //TODO: Fonts
    //TODO: Hot reloading
    //TODO: Search for embedded resources as well
    /*
     * Global Resource Management system
     */
    class ResourceManager {
        public:

            /// Adds material to manager cache
            void addMaterial(Material* material);
            Material* loadPBRMaterial(const std::string& id,
                    const std::string& albedoMapFile,
                    const std::string& normalMapFile,
                    const std::string& roughnessMapFile,
                    const std::string& metallicMapFile,
                    const std::string& aoMapFile);
            /// Load 3D model to manager cache, report if it isn't found
            Model* loadModel(const std::string& fileName, std::string id);
            /// Load texture to manager cache, report if it isn't found
            Texture* loadTexture(
                    std::string fileName,
                    std::string id,
                    GLint wrappingMode = GL_CLAMP_TO_EDGE,
                    bool gammaCorrected = true);
            /// Load .hdr environmental map to manager cache, report if it isn't found
            HDRMap* loadHDREnvironmentalMap(const std::string& fileName, std::string id);

            /// Load combined shader file with specified id
            Shader* loadShader(const std::string& fileName, std::string id);
            /// Load separate vertex and fragment shader files with specified id
            Shader* loadShader(const std::string& vertPath, const std::string& fragPath, std::string id);
            //Shader* loadShader(const std::string& vertPath, const std::string& fragPath, const std::string& geoPath, std::string id) {}
            /// Load audio file and report any errors
            AudioBuffer* loadAudio(const std::string& fileName, std::string id);

            /// Sets custom search direcotires to include in search for all types of resources
            void setSearchDirectories(const std::vector<std::string>& directories) { searchDirectories = directories; }
            /// Adds custom search directory to the list to include in search for all types of resources
            void addSearchDirectory(const std::string& directory) { searchDirectories.push_back(directory); }

            /// Retrieve material from manager cache if any
            Material* getMaterial(const std::string& id);
            /// Retrieve 3D model from manager cache if it exists
            Model* getModel(const std::string& id);
            /// Retrieve shader from manager cache if it exists
            Shader* getShader(const std::string& id);
            /// Retrieve texture from manager cache if it exists
            Texture* getTexture(const std::string& id);
            /// Retrieve HDR map from manager cache if it exists
            HDRMap* getHDR(const std::string& id);
            /// Retrieve audio from manager cache if it exists
            AudioBuffer* getAudio(const std::string& id);

            /// Retrieve singleton instance
            static ResourceManager* Instance() {
                if(sInstance == 0) {
                    sInstance = new ResourceManager();
                    return sInstance;
                }
                return sInstance;
            }

            std::map<std::string, Material*>& getMaterialMap() { return materialMap; }
            void clearMaterialMap();
            void clearMaterial(std::string id);

            std::map<std::string, Model*>& getModelMap() { return modelMap; }
            void clearModelMap();
            void clearModel(std::string id);

            std::map<std::string, Shader*>& getShaderMap() { return shaderMap; }
            void clearShaderMap();
            void clearShader(std::string id);

            std::map<std::string, Texture*>& getTextureMap() { return textureMap; }
            void clearTextureMap();
            void clearTexture(std::string id);

            std::map<std::string, HDRMap*>& getHDRMap() { return hdrMap; }
            void clearHDRMap();
            void clearHDR(std::string id);

            std::map<std::string, AudioBuffer*>& getAudioMap() { return audioMap; }
            void clearAudioMap();
            void clearAudio(std::string id);

        private:
            ResourceManager() {}

            std::map<std::string, Material*> materialMap;
            std::map<std::string, Model*> modelMap;
            std::map<std::string, Shader*> shaderMap;
            std::map<std::string, Texture*> textureMap;
            std::map<std::string, HDRMap*> hdrMap;
            std::map<std::string, AudioBuffer*> audioMap;

            /// Resource search directories
            std::vector<std::string> searchDirectories = {
                "", // For absolute paths
                "./",
                "../",
                "assets",
                "assets/audio/",
                "assets/models/",
                "assets/shaders/",
                "assets/textures/",
                "resources",
                "resources/shaders/",
                "resources/textures/",
            };

            static ResourceManager* sInstance;
    };
}

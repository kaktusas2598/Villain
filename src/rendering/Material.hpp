#pragma once

#include "Texture.hpp"
#include "glm/glm.hpp"

#include <string>
#include <map>

namespace Villain {

    class Material {
        public:
            Material(const std::string& name = std::string()) { this->name = name; }
            virtual ~Material() {}

            // NOTE: Should the engine have default textures to display when there is no texture provided?
            Material(
                    const std::string& name,
                    Texture* diffuse,
                    float shininess,
                    Texture* specular = nullptr,
                    Texture* normal = nullptr,
                    Texture* displacement = nullptr,
                    float dispScale = 0.1f, //0.04f
                    float dispBias = 0.0f) :
                diffuseMap(diffuse), specularFactor(shininess), specularMap(specular), normalMap(normal), dispMap(displacement)
            {
                this->name = name;
                this->dispMapScale = dispScale;
                float baseBias = dispMapScale/2;
                this->dispMapBias = -baseBias + baseBias * dispBias;
            }

            const std::string& getName() const { return name; }
            inline float getSpecularFactor() const { return specularFactor; }
            inline float getDispMapScale() const { return dispMapScale; }
            inline float getDispMapBias() const { return dispMapBias; }
            const glm::vec4& getAmbientColor() { return ambientColor; }
            const glm::vec4& getDiffuseColor() { return diffuseColor; }
            const glm::vec4& getSpecularColor() { return specularColor; }

            void setSpecularFactor(float specular) { specularFactor = specular; }
            void setAmbientColor(const glm::vec4& ambient) { ambientColor = ambient; }
            void setDiffuseColor(const glm::vec4& diffuse) { diffuseColor = diffuse; }
            void setSpecularColor(const glm::vec4& specular) { specularColor = specular; }

            Texture* getDiffuseMap() { return diffuseMap; }
            Texture* getSpecularMap() { return specularMap; }
            Texture* getNormalMap() { return normalMap; }
            Texture* getDislacementMap() { return dispMap; }
        private:
            std::string name; //<<< Material name
            float specularFactor ; //<<< shininess, higher value makes material more reflective and specular highlight becomes smaller
            glm::vec4 diffuseColor{1.0f};
            glm::vec4 ambientColor{1.0f};
            glm::vec4 specularColor{1.0f};
            Texture* diffuseMap = nullptr; //<<< Base colour, diffuse map
            Texture* specularMap = nullptr;//<<< Specularity map
            Texture* normalMap = nullptr; //<<< Normal/bump map
            Texture* dispMap = nullptr; //<<< Parralax displacement map
            float dispMapScale = 0.1f; //<<< Displacement map scaling
            float dispMapBias = 0.0f; //<<< Displacement map offset/bias
    };
}

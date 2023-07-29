#pragma once

#include "Texture.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <string>
#include <map>

namespace Villain {

    /// Main material classes used to bundle material properties and texture maps together
    // TODO: Possibly should also contain shader
    class Material {
        public:
            Material(const std::string& name = std::string()) { this->name = name; }
            virtual ~Material() {}

            /// Material equality based on their name alone to force adding unique names
            bool operator==(const Material& other) const {
                return this->name == other.name;
            }

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
            float* getAmbientColorPtr() { return glm::value_ptr(ambientColor); }
            float* getDiffuseColorPtr() { return glm::value_ptr(diffuseColor); }
            float* getSpecularColorPtr() { return glm::value_ptr(specularColor); }
            float* getSpecularFactorPtr() { return &specularFactor; }
            Texture* getDiffuseMap() { return diffuseMap; }
            Texture* getSpecularMap() { return specularMap; }
            Texture* getNormalMap() { return normalMap; }
            Texture* getDislacementMap() { return dispMap; }

            void setSpecularFactor(float specular) { specularFactor = specular; }
            void setAmbientColor(const glm::vec4& ambient) { ambientColor = ambient; }
            void setDiffuseColor(const glm::vec4& diffuse) { diffuseColor = diffuse; }
            void setSpecularColor(const glm::vec4& specular) { specularColor = specular; }
            void setDiffuseMap(Texture* diffuse) { diffuseMap = diffuse; }

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

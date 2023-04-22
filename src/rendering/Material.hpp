#ifndef __MATERIAL__
#define __MATERIAL__

#include "Texture.hpp"
#include "glm/glm.hpp"

#include <string>
#include <map>

namespace Villain {

    class Material {
        public:
            Material(const std::string& name = std::string()) { this->name = name; }
            virtual ~Material() {}

            // TODO: engine should have default textures to display when there is no texture provided?
            Material(
                    const std::string& name,
                    Texture* diffuse,
                    float shininess,
                    Texture* specular = nullptr,
                    Texture* normal = nullptr,
                    Texture* displacement = nullptr) :
                diffuseMap(diffuse), specularFactor(shininess), specularMap(specular), normalMap(normal), dispMap(displacement)
            {
                this->name = name;
            }

            const std::string& getName() { return name; }

            float getSpecularFactor() { return specularFactor; }
            void setSpecularFactor(float specular) { specularFactor = specular; }

            const glm::vec4& getDiffuseColor() { return diffuseColor; }
            void setDiffuseColor(const glm::vec4& diffuse) { diffuseColor = diffuse; }

            Texture* getDiffuseMap() { return diffuseMap; }
            Texture* getSpecularMap() { return specularMap; }
            Texture* getNormalMap() { return normalMap; }
            Texture* getDislacementMap() { return dispMap; }
        private:
            std::string name; //<<< Material name
            float specularFactor ; //<<< shininess
            glm::vec4 diffuseColor = glm::vec4(1.0f);
            Texture* diffuseMap = nullptr; //<<< Base colour, diffuse map
            Texture* specularMap = nullptr;//<<< Specularity map
            Texture* normalMap = nullptr; //<<< Normal/bump map
            Texture* dispMap = nullptr; //<<< Parralax displacement map
    };
}

#endif // __MATERIAL__

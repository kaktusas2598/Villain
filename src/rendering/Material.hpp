#ifndef __MATERIAL__
#define __MATERIAL__

#include "Texture.hpp"
#include "glm/glm.hpp"

#include <string>
#include <map>

namespace Villain {

    class Material {
        public:
            Material(const std::string& name = std::string());
            virtual ~Material() {}

            // TODO: engine should have default textures to display when there is no texture provided
            Material(
                    const std::string& name,
                    std::vector<Texture*> diffuse,
                    float shininess,
                    std::vector<Texture*> specular = std::vector<Texture*>(),
                    std::vector<Texture*> normal = std::vector<Texture*>());

            const std::string& getName() { return name; }

            float getSpecularFactor() { return specularFactor; }
            void setSpecularFactor(float specular) { specularFactor = specular; }

            const glm::vec4& getDiffuseColor() { return diffuseColor; }
            void setDiffuseColor(const glm::vec4& diffuse) { diffuseColor = diffuse; }

            std::vector<Texture*>& getDiffuseMaps() { return diffuseMaps; }
            std::vector<Texture*>& getSpecularMaps() { return specularMaps; }
            std::vector<Texture*>& getNormalMaps() { return normalMaps; }
        private:
            std::string name; //<<< Material name
            float specularFactor ; //<<< shininess
            glm::vec4 diffuseColor = glm::vec4(1.0f);
            std::vector<Texture*> diffuseMaps;
            std::vector<Texture*> specularMaps;
            std::vector<Texture*> normalMaps;
    };
}

#endif // __MATERIAL__

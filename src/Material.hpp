#ifndef __MATERIAL__
#define __MATERIAL__

#include "Texture.hpp"

#include <string>
#include <map>

namespace Villain {

    // TODO: finish implementing and test this
    class Material {
        public:
            Material(const std::string& name = std::string());
            Material(const Material& other);
            virtual ~Material();

            // TODO: engine should have default textures to display when there is no texture provided
            Material(
                    const std::string& name,
                    Texture* diffuse,
                    float shininess,
                    Texture* specular = nullptr,
                    Texture* normal = nullptr);

            const std::string& getName() { return name; }

            float getSpecularFactor() { return specularFactor; }
            void setSpecularFactor(float specular) { specularFactor = specular; }

            Texture* getDiffuseMap() const { return diffuseMap; }
            Texture* getSpecularMap() const { return specularMap; }
            Texture* getNormalMap() const { return normalMap; }
        private:
            std::string name; //<<< Material name
            float specularFactor ; //<<< shininess
            // NOTE: should material have a vector of textures?
            Texture* diffuseMap; //<<< display texture
            Texture* specularMap = nullptr; //<<< optional specular map
            Texture* normalMap = nullptr; //<<< optional bump map
    };
}

#endif // __MATERIAL__

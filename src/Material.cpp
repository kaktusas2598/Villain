#include "Material.hpp"

namespace Villain {

    Material::Material(const std::string& name) {
        this->name = name;
    }

    Material::Material(
            const std::string& name,
            std::vector<Texture*> diffuse,
            float shininess,
            std::vector<Texture*> specular,
            std::vector<Texture*> normal): diffuseMaps(diffuse), specularFactor(shininess), specularMaps(specular), normalMaps(normal)
    {
        this->name = name;
    }

    Material::~Material() { }
}

#include "Material.hpp"

namespace Villain {

    Material::Material(const std::string& name) {
        this->name = name;
    }

    Material::Material(const Material& other) :
        name(other.name), specularMap(other.specularMap), specularFactor(other.specularFactor),
        diffuseMap(other.diffuseMap), normalMap(other.normalMap)
    { }

    Material::~Material() { }

    Material::Material(
            const std::string& name,
            Texture* diffuse,
            float shininess,
            Texture* specular,
            Texture* normal) : diffuseMap(diffuse), specularFactor(shininess), specularMap(specular), normalMap(normal)
    {
        this->name = name;
    }

}

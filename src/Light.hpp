#ifndef __LIGHT__
#define __LIGHT__

#include <glm/glm.hpp>

namespace Villain {

    /*
     * Stores light source definitions
     */

    // Base color components used by all light sources
    struct LightColor {
        glm::vec3 AmbientColor;
        glm::vec3 DiffuseColor;
        glm::vec3 SpecularColor;

        LightColor(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular) :
            AmbientColor(ambient), DiffuseColor(diffuse), SpecularColor(specular) {}
    };

    struct DirectionalLight {
        LightColor Base;
        glm::vec3 Direction;

        DirectionalLight(LightColor& lightColors, const glm::vec3& dir) :
            Base(lightColors), Direction(dir) {}
    };

    struct PointLight {
        LightColor Base;
        glm::vec3 Position;
        // Attenuation factors
        float Constant;
        float Linear;
        float Quadratic;

        PointLight(LightColor& lightColors, const glm::vec3& pos, float cnst, float linr, float quadr) :
            Base(lightColors), Position(pos), Constant(cnst), Linear(linr), Quadratic(quadr) {}
    };

    struct SpotLight {
        LightColor Base;
        glm::vec3 Position;
        glm::vec3 Direction;
        float CutOff;
        float OuterCutOff;

        SpotLight(LightColor& lightColors, const glm::vec3& pos, const glm::vec3& dir, float cutOff, float outerCutOff) :
            Base(lightColors), Position(pos), Direction(dir), CutOff(cutOff), OuterCutOff(outerCutOff) {}
    };

}

#endif // __LIGHT__

#ifndef __LIGHT__
#define __LIGHT__

#include <glm/glm.hpp>

namespace Villain {

    /*
     * Stores light source definitions
     */

    // TODO: refactor all of this to classes and all light sources externing BaseLight
    // and they can provide methods to set uniforms as well, then add these to rendering engine
    // and render using multi-pass forward rendering system for many lights at once
    //
    // Base color components used by all light sources
    struct BaseLight {
        glm::vec3 AmbientColor;
        glm::vec3 DiffuseColor;
        glm::vec3 SpecularColor;

        BaseLight(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular) :
            AmbientColor(ambient), DiffuseColor(diffuse), SpecularColor(specular) {}
    };

    struct DirectionalLight {
        BaseLight Base;
        glm::vec3 Direction;

        DirectionalLight(BaseLight& lightColors, const glm::vec3& dir) :
            Base(lightColors), Direction(dir) {}
    };

    struct PointLight {
        BaseLight Base;
        glm::vec3 Position;
        // Attenuation factors
        float Constant;
        float Linear;
        float Quadratic;

        PointLight(BaseLight& lightColors, const glm::vec3& pos, float cnst, float linr, float quadr) :
            Base(lightColors), Position(pos), Constant(cnst), Linear(linr), Quadratic(quadr) {}
    };

    struct SpotLight {
        BaseLight Base;
        glm::vec3 Position;
        glm::vec3 Direction;
        float CutOff;
        float OuterCutOff;

        SpotLight(BaseLight& lightColors, const glm::vec3& pos, const glm::vec3& dir, float cutOff, float outerCutOff) :
            Base(lightColors), Position(pos), Direction(dir), CutOff(cutOff), OuterCutOff(outerCutOff) {}
    };

}

#endif // __LIGHT__

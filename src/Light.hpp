#ifndef __LIGHT__
#define __LIGHT__

#include <glm/glm.hpp>
#include "NodeComponent.hpp"

namespace Villain {

    // TODO: Specular color can be changed by specular intensity probably, same with diffuse?
    // Because it feels strange passing 3 colour values just to setup a light
    //
    // Base color components used by all light sources
    class BaseLight : public NodeComponent {
        public:
            glm::vec3 AmbientColor;
            glm::vec3 DiffuseColor;
            glm::vec3 SpecularColor;

            BaseLight(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular) :
                AmbientColor(ambient), DiffuseColor(diffuse), SpecularColor(specular) {}
            ~BaseLight();
            virtual void addToEngine(Engine* engine) override;
            virtual std::string type() = 0;

            Shader* getShader() { return shader; }
        protected:
            Shader* shader;
    };

    class DirectionalLight : public BaseLight {
        public:
            glm::vec3 Direction;

            DirectionalLight(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, const glm::vec3& dir);
            virtual std::string type() { return std::string("directional"); }
    };

    class PointLight : public BaseLight {
        public:
            glm::vec3 Position;
            // Attenuation factors
            float Constant;
            float Linear;
            float Quadratic;

            PointLight(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, const glm::vec3& pos, float cnst, float linr, float quadr);
            virtual std::string type() { return std::string("point"); }
    };

    class SpotLight : public BaseLight {
        public:
            glm::vec3 Position;
            glm::vec3 Direction;
            float CutOff;
            float OuterCutOff;

            SpotLight(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, const glm::vec3& pos, const glm::vec3& dir, float cutOff, float outerCutOff);
            virtual std::string type() { return std::string("spot"); }
    };

}

#endif // __LIGHT__

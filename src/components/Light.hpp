#ifndef __LIGHT__
#define __LIGHT__

#include <glm/glm.hpp>
#include "Camera3D.hpp"
#include "NodeComponent.hpp"

namespace Villain {

    class ShadowInfo {
        public:
            ShadowInfo(const glm::mat4& proj): projection(proj) {}

            inline glm::mat4 getProjection() const { return projection; }
        private:
            glm::mat4 projection;
    };

    // TODO:
    // 1. Specular color can be changed by specular intensity probably, same with diffuse?
    // Because it feels strange passing 3 colour values just to setup a light
    // 2. Spot light needs attenuation
    //
    // Base color components used by all light sources
    class BaseLight : public NodeComponent {
        public:
            glm::vec3 AmbientColor;
            glm::vec3 DiffuseColor;
            glm::vec3 SpecularColor;

            BaseLight(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular) :
                AmbientColor(ambient), DiffuseColor(diffuse), SpecularColor(specular), shader(nullptr), shadowInfo(nullptr) {}
            ~BaseLight();
            virtual void addToEngine(Engine* engine) override;
            virtual std::string type() = 0;
            inline ShadowInfo* getShadowInfo() const { return shadowInfo; }
            inline Shader* getShader() { return shader; }
        protected:
            void setShadowInfo(ShadowInfo* info);
            Shader* shader;
        private:
            ShadowInfo* shadowInfo;
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
            virtual void update(float deltaTime);
    };

    class SpotLight : public BaseLight {
        public:
            glm::vec3 Position;
            glm::vec3 Direction;
            float CutOff;
            float OuterCutOff;
            Camera3D* camera = nullptr;

            SpotLight(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, const glm::vec3& pos, const glm::vec3& dir, float cutOff, float outerCutOff, Camera3D* cam = nullptr);
            virtual std::string type() { return std::string("spot"); }
            virtual void update(float deltaTime);
    };

}

#endif // __LIGHT__

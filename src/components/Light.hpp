#pragma once

#include <glm/glm.hpp>
#include "NodeComponent.hpp"

namespace Villain {

    class ShadowInfo {
        public:
            ShadowInfo(const glm::mat4& proj, float bias = 4.f, bool flipCullFaces = true)
                : projection(proj), shadowBias(bias), flipFaces(flipCullFaces) {}

            void setFarPlane(float newFar) { farPlane = newFar; }

            inline glm::mat4 getProjection() const { return projection; }
            inline float getBias() const { return shadowBias; }
            inline bool getFlipFaces() const { return flipFaces; }
            inline float getFarPlane() const { return farPlane; }

            inline float* getBiasPointer() { return &shadowBias; }
        private:
            glm::mat4 projection;
            // Shadow map resolution-indepentent bias, so with default shadow map size of 1024 and bias of 1.f
            // actual bias will be 1.f /1024 = 0.001f
            float shadowBias; // Adjust to reduce/remove shadow acne artifacts
            bool flipFaces; // Turn on to solve peter panning artifacts
            float farPlane; // Only used for omnidirectional shadow mapping
    };

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
            // X - Constant, Y - Linear, Z - Quadratic
            glm::vec3 Attenuation;

            PointLight(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, const glm::vec3& pos, const glm::vec3& attenuation = {1.0f, 0.022f, 0.0019f});
            virtual std::string type() { return std::string("point"); }
            virtual void update(float deltaTime);
    };

    class SpotLight : public BaseLight {
        public:
            glm::vec3 Position;
            glm::vec3 Direction;
            float CutOff;
            float OuterCutOff;
            // X - Constant, Y - Linear, Z - Quadratic
            glm::vec3 Attenuation;
            Camera* camera = nullptr;

            SpotLight(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, const glm::vec3& pos, const glm::vec3& dir, float cutOff, float outerCutOff, const glm::vec3& attenuation = {1.0f, 0.022f, 0.0019f}, Camera* cam = nullptr);
            virtual std::string type() { return std::string("spot"); }
            virtual void update(float deltaTime);
    };
}

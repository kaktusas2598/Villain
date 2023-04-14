#include "Light.hpp"

#include "Engine.hpp"
#include "../rendering/Shader.hpp"

namespace Villain {

    BaseLight::~BaseLight() {
        delete shader;
    }

    void BaseLight::addToEngine(Engine* engine) {
        engine->getRenderingEngine()->addLight(this);
    }

    DirectionalLight::DirectionalLight(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, const glm::vec3& dir) :
        BaseLight(ambient, diffuse, specular), Direction(dir) {

        shader = new Shader();
        shader->createFromResource("forward-directional");
    }

    PointLight::PointLight(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, const glm::vec3& pos, float cnst, float linr, float quadr) :
                BaseLight(ambient, diffuse, specular), Position(pos), Constant(cnst), Linear(linr), Quadratic(quadr) {

        shader = new Shader();
        shader->createFromResource("forward-point");
    }

    void PointLight::update(float deltaTime) {
        Position = GetTransform()->getPos();
    }

    SpotLight::SpotLight(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, const glm::vec3& pos, const glm::vec3& dir, float cutOff, float outerCutOff, Camera3D* cam) :
                BaseLight(ambient, diffuse, specular), Position(pos), Direction(dir), CutOff(cutOff), OuterCutOff(outerCutOff), camera(cam) {

        shader = new Shader();
        shader->createFromResource("forward-spot");
    }

    void SpotLight::update(float deltaTime) {
        Position = GetTransform()->getPos();

        if (camera != nullptr) {
            Position = camera->getPosition();
            Direction = camera->getFront();
        }
    }
}

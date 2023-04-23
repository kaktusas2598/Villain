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

    void BaseLight::setShadowInfo(ShadowInfo* info) {
        if (shadowInfo) delete shadowInfo;
        shadowInfo = info;
    }

    DirectionalLight::DirectionalLight(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, const glm::vec3& dir) :
        BaseLight(ambient, diffuse, specular), Direction(dir) {

        shader = Shader::createFromResource("forward-directional");

        setShadowInfo(new ShadowInfo(glm::ortho(-40, 40, -40, 40, -40, 40)));
    }

    PointLight::PointLight(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, const glm::vec3& pos, float cnst, float linr, float quadr) :
                BaseLight(ambient, diffuse, specular), Position(pos), Constant(cnst), Linear(linr), Quadratic(quadr) {

        shader = Shader::createFromResource("forward-point");
    }

    void PointLight::update(float deltaTime) {
        Position = GetTransform()->getPos();
    }

    SpotLight::SpotLight(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, const glm::vec3& pos, const glm::vec3& dir, float cutOff, float outerCutOff, Camera3D* cam) :
                BaseLight(ambient, diffuse, specular), Position(pos), Direction(dir), CutOff(cutOff), OuterCutOff(outerCutOff), camera(cam) {

        shader = Shader::createFromResource("forward-spot");
    }

    void SpotLight::update(float deltaTime) {
        Position = GetTransform()->getPos();

        if (camera != nullptr) {
            Position = camera->getPosition();
            Direction = camera->getFront();
        }
    }
}

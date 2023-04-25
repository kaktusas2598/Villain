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

        // NOTE: messing with light projection completely messes up shadows if not set correctly
        // all objects lit by this light must be included in light frustum
        //setShadowInfo(new ShadowInfo(glm::ortho(-40., 40., -40., 40., 0.1, 75.)));
        //setShadowInfo(new ShadowInfo(glm::ortho(-10., 10., -10., 10., 0.1, 7.5)));
        //setShadowInfo(new ShadowInfo(glm::ortho(-100., 100., -100., 100., 0.1, 75.)));
        setShadowInfo(new ShadowInfo(glm::ortho(-100., 100., -100., 100., 0.1, 100.)));
    }

    PointLight::PointLight(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, const glm::vec3& pos, const glm::vec3& attenuation) :
                BaseLight(ambient, diffuse, specular), Position(pos), Attenuation(attenuation) {

        shader = Shader::createFromResource("forward-point");
        // TODO: shadow mapping implementation, use cubemap textures
    }

    void PointLight::update(float deltaTime) {
        Position = GetTransform()->getPos();
    }

    SpotLight::SpotLight(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, const glm::vec3& pos, const glm::vec3& dir, float cutOff, float outerCutOff, const glm::vec3& attenuation, Camera3D* cam) :
                BaseLight(ambient, diffuse, specular), Position(pos), Direction(dir), CutOff(cutOff), OuterCutOff(outerCutOff), Attenuation(attenuation) {

        shader = Shader::createFromResource("forward-spot");
        // Shadow mapping for spot lights is almost identical to directional lights, but it uses perspective projection
        // Aspect ratio is 1 because shadow map texture is a rectangle
        // TODO: Zfar plane should probably be same as camera range(use attenuation to calculate)
        setShadowInfo(new ShadowInfo(glm::perspective((double)glm::acos(outerCutOff), 1., 1.0, 50.)));
    }

    void SpotLight::update(float deltaTime) {
        Position = GetTransform()->getPos();

        if (camera != nullptr) {
            Position = camera->getPosition();
            Direction = camera->getFront();
        }
    }
}

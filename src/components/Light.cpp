#include "Light.hpp"

#include "Engine.hpp"
#include "../rendering/Shader.hpp"

namespace Villain {

    BaseLight::~BaseLight() {
        delete shader;
        getParent()->getEngine()->getRenderingEngine()->removeLight(this);
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
        VILLAIN_SET_COMPONENT_ID(DirectionalLight);

        shader = Shader::createFromResource("forward-directional");

        // NOTE: view camera could also be used to calculate shadow map transformation from the eye's POV
        // TODO: add ability to set shadow area - used in setting up projection frustum
        setShadowInfo(new ShadowInfo(glm::ortho(-100., 100., -100., 100., 0.1, 100.)));
    }

    void DirectionalLight::setUniforms(Shader& shaderProgram) {
        // HACK: for shadow mapping, cause technically dir lights have no position
        shaderProgram.setUniformVec3("dirLight.position", GetTransform()->getPos());

        shaderProgram.setUniformVec3("dirLight.direction", Direction);
        shaderProgram.setUniformVec3("dirLight.base.ambient", AmbientColor);
        shaderProgram.setUniformVec3("dirLight.base.diffuse", DiffuseColor);
        shaderProgram.setUniformVec3("dirLight.base.specular", SpecularColor);
    }

    PointLight::PointLight(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, const glm::vec3& pos, const glm::vec3& attenuation) :
                BaseLight(ambient, diffuse, specular), Position(pos), Attenuation(attenuation) {
        VILLAIN_SET_COMPONENT_ID(PointLight);

        shader = Shader::createFromResource("forward-point");
        // TODO: Zfar plane should probably be same as camera range(use attenuation to calculate)
        setShadowInfo(new ShadowInfo(glm::perspective((double)glm::radians(90.0f), 1., 1.0, 50.)));
        getShadowInfo()->setFarPlane(50.0f); // Not nice approach here, as farPlane is only needed for point light shadow mapping
    }

    void PointLight::setUniforms(Shader& shaderProgram) {
        shaderProgram.setUniformVec3("pointLight.position", Position);
        shaderProgram.setUniform1f("pointLight.constant", Attenuation.x);
        shaderProgram.setUniform1f("pointLight.linear", Attenuation.y);
        shaderProgram.setUniform1f("pointLight.quadratic", Attenuation.z);
        shaderProgram.setUniformVec3("pointLight.base.ambient", AmbientColor);
        shaderProgram.setUniformVec3("pointLight.base.diffuse", DiffuseColor);
        shaderProgram.setUniformVec3("pointLight.base.specular", SpecularColor);
    }

    void PointLight::update(float deltaTime) {
        Position = GetTransform()->getPos();
    }

    SpotLight::SpotLight(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, const glm::vec3& pos, const glm::vec3& dir, float cutOff, float outerCutOff, const glm::vec3& attenuation, Camera* cam) :
                BaseLight(ambient, diffuse, specular), Position(pos), Direction(dir), CutOff(cutOff), OuterCutOff(outerCutOff), Attenuation(attenuation), camera(cam) {
        VILLAIN_SET_COMPONENT_ID(SpotLight);

        shader = Shader::createFromResource("forward-spot");
        // Shadow mapping for spot lights is almost identical to directional lights, but it uses perspective projection
        // Aspect ratio is 1 because shadow map texture is a rectangle
        // TODO: Zfar plane should probably be same as camera range(use attenuation to calculate)
        setShadowInfo(new ShadowInfo(glm::perspective((double)glm::acos(outerCutOff), 1., 1.0, 50.)));
    }

    void SpotLight::setUniforms(Shader& shaderProgram) {
        shaderProgram.setUniformVec3("spotLight.position", Position);
        shaderProgram.setUniformVec3("spotLight.direction", Direction);
        shaderProgram.setUniform1f("spotLight.cutOff", CutOff);
        shaderProgram.setUniform1f("spotLight.outerCutOff", OuterCutOff);
        shaderProgram.setUniform1f("spotLight.constant", Attenuation.x);
        shaderProgram.setUniform1f("spotLight.linear", Attenuation.y);
        shaderProgram.setUniform1f("spotLight.quadratic", Attenuation.z);
        shaderProgram.setUniformVec3("spotLight.base.ambient", AmbientColor);
        shaderProgram.setUniformVec3("spotLight.base.diffuse", DiffuseColor);
        shaderProgram.setUniformVec3("spotLight.base.specular", SpecularColor);
    }

    void SpotLight::update(float deltaTime) {
        Position = GetTransform()->getPos();

        if (camera != nullptr) {
            Position = camera->getPosition();
            Direction = camera->getFront();
        } else {
        }
    }
}

#include "ParticleEmitter.hpp"
#include "InputManager.hpp"

#include <glm/gtx/string_cast.hpp>

namespace Villain {
    void ParticleEmitter::render(Shader& shader, RenderingEngine& renderingEngine, Camera& camera) {
        for (int i = 0; i < poolSize; ++i) {
            // Create a translation matrix based on the particle's position
            glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), particleArray[i].getPosition());
            instanceMatrices[i] = translationMatrix;
        }


        particleQuadMesh->updateInstances(instanceMatrices);

        Material material;
        shader.updateUniforms(*parent->getTransform(), material, renderingEngine, camera);
        particleQuadMesh->draw(shader, material);

    }

    // NOTE: all code here apart from integration is temporary!!!
    void ParticleEmitter::update(float deltaTime) {
        for (size_t i = 0; i < poolSize; i++) {
            // NOTE: temp add gravity for testing
            particleArray[i].setAcceleration(glm::vec3(0.0, -1.0f, 0.0f));
            particleArray[i].integrate(deltaTime);

            // NOTE: rudimentary collision, temporary
            if (particleArray[i].getPosition().y < 0.0f) {
                particleArray[i].setPosition(glm::vec3(0.0f, 4.0f, 0.0f));
            }
        }
    }

    void ParticleEmitter::handleInput(float deltaTime) {
        if (InputManager::Instance()->isKeyPressed(SDLK_p)) {
            particleType++;
            particleType = particleType % 4;
            switch (particleType) {
                case 0:
                    // Pistol
                    for (int i = 0; i < poolSize; i++) {
                        particleArray[i].setMass(2.0f); // 2kg
                        particleArray[i].setPosition(glm::vec3(0.0f, 4.0f, 0.0f));
                        particleArray[i].setVelocity(glm::vec3(0.0f, 0.0f, 35.0f)); // 35 m/s
                        particleArray[i].setDamping(0.99f);
                    }
                    break;
                case 1:
                    // Artillery
                    for (int i = 0; i < poolSize; i++) {
                        particleArray[i].setMass(200.0f);
                        particleArray[i].setPosition(glm::vec3(0.0f, 4.0f, 0.0f));
                        particleArray[i].setVelocity(glm::vec3(0.0f, 30.0f, 40.0f));
                        particleArray[i].setDamping(0.99f);
                    }
                    break;
                case 2:
                    // Fireball
                    for (int i = 0; i < poolSize; i++) {
                        particleArray[i].setMass(1.0f);
                        particleArray[i].setPosition(glm::vec3(0.0f, 4.0f, 0.0f));
                        particleArray[i].setVelocity(glm::vec3(0.0f, 0.0f, 10.0f));
                        particleArray[i].setDamping(0.9f);
                    }
                    break;
                case 3:
                    // Laser
                    for (int i = 0; i < poolSize; i++) {
                        particleArray[i].setMass(0.1f);
                        particleArray[i].setPosition(glm::vec3(0.0f, 4.0f, 0.0f));
                        particleArray[i].setVelocity(glm::vec3(0.0f, 0.0f, 100.0f));
                        particleArray[i].setDamping(0.99f);
                    }
                    break;
                default:
                    printf("Unsupported particle type\n");
                    break;
            }
        }
    }

};

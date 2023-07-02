#include "ParticleEmitter.hpp"
#include "InputManager.hpp"

#include "physics/generators/ParticleAnchoredSpring.hpp"
#include "physics/generators/ParticleGravity.hpp"
#include "physics/generators/ParticleSpring.hpp"

#include <glm/gtx/string_cast.hpp>

namespace Villain {

    void ParticleEmitter::initialiseForceGenerators() {
        ParticleGravity* gravityGenerator = new ParticleGravity(glm::vec3(0.0f, -1.0f, 0.0f));

        // Attach gravity generator to each particle except spring ones
        for (int i = 3; i < poolSize; i++) {
            registry.add(&particleArray[i], gravityGenerator);
        }

        particleArray[springAParticleIndex].setPosition({0.0f, 10.0f, 0.0f});
        particleArray[springBParticleIndex].setPosition({0.0f, 5.0f, 0.0f});

        ParticleSpring* psA = new ParticleSpring(&particleArray[springBParticleIndex], 1.0f, 2.0f);
        registry.add(&particleArray[springAParticleIndex], psA);

        ParticleSpring* psB = new ParticleSpring(&particleArray[springAParticleIndex], 1.0f, 2.0f);
        registry.add(&particleArray[springBParticleIndex], psB);

        particleArray[anchoredParticleIndex].setPosition({10.0f, 5.0f, 0.0f});
        ParticleAnchoredSpring* anchoredSpring = new ParticleAnchoredSpring({10.0f, 15.0f, 0.0f}, 1.0, 2.0f);
        registry.add(&particleArray[anchoredParticleIndex], anchoredSpring);

    }

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

    void ParticleEmitter::update(float deltaTime) {
        // Do not forget to update force generator registry!
        registry.updateForces(deltaTime);

        for (size_t i = 0; i < poolSize; i++) {
            particleArray[i].integrate(deltaTime);

            // NOTE: rudimentary collision, temporary!!!
            if (particleArray[i].getPosition().y < 0.0f) {
                setParticleType((ParticleType)particleType);
            }
        }
    }

    void ParticleEmitter::handleInput(float deltaTime) {
        if (InputManager::Instance()->isKeyPressed(SDLK_p)) {
            particleType++;
            particleType = particleType % 4;
            setParticleType((ParticleType)particleType);
        }
    }

    // TEMP method
    void ParticleEmitter::setParticleType(ParticleType newType) {
        switch (newType) {
            case ParticleType::PISTOL:
                // Pistol
                for (int i = 3; i < poolSize; i++) {
                    particleArray[i].setMass(2.0f); // 2kg
                    particleArray[i].setPosition(glm::vec3(0.0f, 4.0f, 0.0f));
                    particleArray[i].setVelocity(glm::vec3(0.0f, 0.0f, 35.0f)); // 35 m/s
                    particleArray[i].setDamping(0.99f);
                }
                break;
            case ParticleType::ARTILLERY:
                // Artillery
                for (int i = 3; i < poolSize; i++) {
                    particleArray[i].setMass(200.0f);
                    particleArray[i].setPosition(glm::vec3(0.0f, 4.0f, 0.0f));
                    particleArray[i].setVelocity(glm::vec3(0.0f, 30.0f, 40.0f));
                    particleArray[i].setDamping(0.99f);
                }
                break;
            case ParticleType::FIREBALL:
                // Fireball
                for (int i = 3; i < poolSize; i++) {
                    particleArray[i].setMass(1.0f);
                    particleArray[i].setPosition(glm::vec3(0.0f, 4.0f, 0.0f));
                    particleArray[i].setVelocity(glm::vec3(0.0f, 0.0f, 10.0f));
                    particleArray[i].setDamping(0.9f);
                }
                break;
            case ParticleType::LASER:
                // Laser
                for (int i = 3; i < poolSize; i++) {
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

};

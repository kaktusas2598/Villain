#include "ParticleEmitter.hpp"
#include "InputManager.hpp"

#include "physics/generators/force/ParticleAnchoredBungee.hpp"
#include "physics/generators/force/ParticleAnchoredSpring.hpp"
#include "physics/generators/force/ParticleBungee.hpp"
#include "physics/generators/force/ParticleBuoyancy.hpp"
#include "physics/generators/force/ParticleFakeSpring.hpp"
#include "physics/generators/force/ParticleGravity.hpp"
#include "physics/generators/force/ParticleSpring.hpp"

#include <glm/gtx/string_cast.hpp>

namespace Villain {

    void ParticleEmitter::initialiseForceGenerators() {
        particleArray[springAParticleIndex].setPosition({0.0f, 15.0f, 0.0f});
        particleArray[springBParticleIndex].setPosition({0.0f, 5.0f, 0.0f});
        // 2 particles connected with spring
        ParticleSpring* psA = new ParticleSpring(&particleArray[springBParticleIndex], 1.0f, 2.0f);
        registry.add(&particleArray[springAParticleIndex], psA);
        ParticleSpring* psB = new ParticleSpring(&particleArray[springAParticleIndex], 1.0f, 2.0f);
        registry.add(&particleArray[springBParticleIndex], psB);

        particleArray[anchoredParticleIndex].setPosition({5.0f, 5.0f, 0.0f});
        //// 1 particle anchored by a spring
        ParticleAnchoredSpring* anchoredSpring = new ParticleAnchoredSpring({5.0f, 15.0f, 0.0f}, 1.0, 2.0f);
        registry.add(&particleArray[anchoredParticleIndex], anchoredSpring);

        particleArray[bungeeAParticleIndex].setPosition({10.0f, 15.0f, 0.0f});
        particleArray[bungeeBParticleIndex].setPosition({10.0f, 5.0f, 0.0f});
        // 2 particles connected by a bungee
        ParticleBungee* pbA = new ParticleBungee(&particleArray[bungeeBParticleIndex], 1.0f, 5.0f);
        registry.add(&particleArray[bungeeAParticleIndex], pbA);
        ParticleBungee* pbB = new ParticleBungee(&particleArray[bungeeAParticleIndex], 1.0f, 5.0f);
        registry.add(&particleArray[bungeeBParticleIndex], pbB);

        particleArray[anchoredBungeeParticleIndex].setPosition({15.0f, 5.0f, 0.0f});
        // 1 particle anchored by a bungee
        ParticleAnchoredBungee* anchoredBungee = new ParticleAnchoredBungee({15.0f, 15.0f, 0.0f}, 1.0, 2.0f);
        registry.add(&particleArray[anchoredBungeeParticleIndex], anchoredBungee);

        ParticleGravity* gravityGenerator = new ParticleGravity(glm::vec3(0.0f, -1.0f, 0.0f));

        // Buoyancy test
        // NOTE: if gravity isn't attached particle just floats up to infinity and beyond
        particleArray[buoyancyParticleIndex].setPosition({20.0f, 4.9f, 0.0f});
        ParticleBuoyancy* buoyancy = new ParticleBuoyancy(0.4f, 0.01f, 5.0f);
        registry.add(&particleArray[buoyancyParticleIndex], gravityGenerator);
        registry.add(&particleArray[buoyancyParticleIndex], buoyancy);

        // Attach gravity generator to each particle except spring ones
        // and set initial params to simulate projectiles
        for (int i = projectileParticleStart; i < poolSize; i++) {
            particleArray[i].setMass(2.0f); // 2kg
            particleArray[i].setPosition(glm::vec3(0.0f, 2.0f, 0.0f));
            particleArray[i].setVelocity(glm::vec3(0.0f, 0.0f, 35.0f)); // 35 m/s
            particleArray[i].setDamping(0.99f);

            registry.add(&particleArray[i], gravityGenerator);
        }

        // Fake stiff spring force test
        particleArray[fakeStiffSpringParticleIndex].setPosition({25.0f, 10.0f, 0.0f});
        ParticleFakeSpring* fakeStiffSpring = new ParticleFakeSpring({25.0f, 15.0f, 0.0f}, 2000.f, 0.1f);
        registry.add(&particleArray[fakeStiffSpringParticleIndex], fakeStiffSpring);
    }

    void ParticleEmitter::render(Shader& shader, RenderingEngine& renderingEngine, Camera& camera) {
        for (int i = 0; i < poolSize; ++i) {
            // Create a translation matrix based on the particle's position
            glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), particleArray[i].getPosition());
            instanceMatrices[i] = translationMatrix;
        }
        //Logger::Instance()->info("Bungee A Position {}", glm::to_string(particleArray[bungeeAParticleIndex].getPosition()));
        //Logger::Instance()->info("Bungee B Position {}", glm::to_string(particleArray[bungeeBParticleIndex].getPosition()));
        //Logger::Instance()->info("Anchored Bungee Position {}", glm::to_string(particleArray[anchoredBungeeParticleIndex].getPosition()));

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
            if (i >= projectileParticleStart && particleArray[i].getPosition().y < 0.0f) {
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
                for (int i = projectileParticleStart; i < poolSize; i++) {
                    particleArray[i].setMass(2.0f); // 2kg
                    particleArray[i].setPosition(glm::vec3(0.0f, 4.0f, 0.0f));
                    particleArray[i].setVelocity(glm::vec3(0.0f, 0.0f, 35.0f)); // 35 m/s
                    particleArray[i].setDamping(0.99f);
                }
                break;
            case ParticleType::ARTILLERY:
                // Artillery
                for (int i = projectileParticleStart; i < poolSize; i++) {
                    particleArray[i].setMass(200.0f);
                    particleArray[i].setPosition(glm::vec3(0.0f, 4.0f, 0.0f));
                    particleArray[i].setVelocity(glm::vec3(0.0f, 30.0f, 40.0f));
                    particleArray[i].setDamping(0.99f);
                }
                break;
            case ParticleType::FIREBALL:
                // Fireball
                for (int i = projectileParticleStart; i < poolSize; i++) {
                    particleArray[i].setMass(1.0f);
                    particleArray[i].setPosition(glm::vec3(0.0f, 4.0f, 0.0f));
                    particleArray[i].setVelocity(glm::vec3(0.0f, 0.0f, 10.0f));
                    particleArray[i].setDamping(0.9f);
                }
                break;
            case ParticleType::LASER:
                // Laser
                for (int i = projectileParticleStart; i < poolSize; i++) {
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

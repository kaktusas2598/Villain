#pragma once

#include "Logger.hpp"
#include "NodeComponent.hpp"
//#include "physics/ParticlePool.hpp"
#include "physics/Particle.hpp"
#include "physics/ParticleForceGenerator.hpp"
#include "physics/generators/ParticleGravity.hpp"
#include "rendering/Mesh.hpp"
#include "rendering/MeshUtils.hpp"

namespace Villain {

    enum class ParticleShape {
        QUAD, SPHERE
    };

    // Testing code really, once particle system is finished we won't need this
    // it will be up to game developer to define types of projectiles
    enum class ParticleType {
        NONE = -1,
        PISTOL,
        ARTILLERY,
        FIREBALL,
        LASER
    };

    // NOTE: Just blueprint/sandbox for now until I get something rendering on the screen for new particle system
    // TODO:
    // 1. Change to use particle memory pool
    // 2. Support multiple parameters, or maybe types of particle emitters
    class ParticleEmitter : public NodeComponent {
        public:
            ParticleEmitter(int numParticles, ParticleShape shape = ParticleShape::SPHERE) {
                //pool = new ParticlePool(numParticles);
                poolSize = numParticles;
                particleArray = new Particle[numParticles];

                std::vector<VertexP1N1T1B1UV> vertices;
                std::vector<unsigned int> indices;
                if (shape == ParticleShape::SPHERE) {
                    MeshUtils<VertexP1N1T1B1UV>::addSphere(&vertices, &indices, 1.0f, glm::vec3(0.0f));
                } else if (shape == ParticleShape::QUAD) {
                    MeshUtils<VertexP1N1T1B1UV>::addXYPlane(
                            &vertices, &indices, glm::vec3(0.0f), glm::vec2(1.0f), new float[4]{0.0, 1.0, 0.0, 1.0});
                } else {
                    Logger::Instance()->error("Particle shape not supported");
                }

                ParticleGravity* gravityGenerator = new ParticleGravity(glm::vec3(0.0f, -1.0f, 0.0f));

                instanceMatrices.reserve(numParticles);
                for (int i = 0; i < numParticles; i++) {
                    // Attach gravity generator to each particle
                    registry.add(&particleArray[i], gravityGenerator);
                    particleArray[i].setMass(2.0f); // 2kg
                    particleArray[i].setPosition(glm::vec3(0.0f, 2.0f, 0.0f));
                    particleArray[i].setVelocity(glm::vec3(0.0f, 0.0f, 35.0f)); // 35 m/s
                    particleArray[i].setDamping(0.99f);
                    instanceMatrices.push_back(glm::mat4(1.0f));
                }

                // quad Vertices and Indices gets passed to constructor of new Mesh here so we can pass instancing data to constructor
                particleQuadMesh = new Mesh<VertexP1N1T1B1UV>(vertices, indices, numParticles, instanceMatrices);
            }

            virtual void render(Shader& shader, RenderingEngine& renderingEngine, Camera& camera);
            virtual void update(float deltaTime);
            virtual void handleInput(float deltaTime);

        private:
            // Will host memory pool in here!
            //ParticlePool* pool = nullptr;
            int poolSize = 0;
            Particle* particleArray = nullptr;
            std::vector<glm::mat4> instanceMatrices;

            ParticleForceRegistry registry;

            // TEMP for testing particle attributes for now
            int particleType = 0;
            void setParticleType(ParticleType newType);

            // Inspired by Mesh Renderer
            Mesh<VertexP1N1T1B1UV>* particleQuadMesh;
            Material particleMaterial;

    };
}

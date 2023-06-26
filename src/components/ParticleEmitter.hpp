#ifndef __PARTICLE_EMITTER__
#define __PARTICLE_EMITTER__

#include "NodeComponent.hpp"
//#include "physics/ParticlePool.hpp"
#include "rendering/Mesh.hpp"
#include "rendering/MeshUtils.hpp"

namespace Villain {

    // NOTE: Just blueprint/sandbox for now until I get something rendering on the screen for new particle system
    // TODO:
    // 1. Change to use particle memory pool
    // 2. Support multiple parameters, or maybe types of particle emitters
    class ParticleEmitter : public NodeComponent {
        public:
            ParticleEmitter(int numParticles) {
                //pool = new ParticlePool(numParticles);
                poolSize = numParticles;
                particleArray = new Particle[numParticles];

                std::vector<VertexP1N1T1B1UV> vertices;
                vertices.reserve(4);

                instanceMatrices.reserve(numParticles);
                for (int i = 0; i < numParticles; i++) {
                    particleArray[i].setMass(2.0f); // 2kg
                    particleArray[i].setPosition(glm::vec3(0.0f, 2.0f, 0.0f));
                    particleArray[i].setVelocity(glm::vec3(0.0f, 0.0f, 35.0f)); // 35 m/s
                    particleArray[i].setDamping(0.99f);
                    instanceMatrices.push_back(glm::mat4(1.0f));
                }
                Mesh<VertexP1N1T1B1UV>* quad = MeshUtils<VertexP1N1T1B1UV>::getXYPlane(
                        glm::vec3(0.0f), glm::vec2(1.0f), new float[4]{0.0, 1.0, 0.0, 1.0});

                // quad Vertices and Indices gets passed to constructor of new Mesh here so we can pass instancing data to constructor
                particleQuadMesh = new Mesh<VertexP1N1T1B1UV>(quad->Vertices, quad->Indices, numParticles, instanceMatrices);
                delete quad;
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

            // TEMP
            int particleType = 0;

            // Inspired by Mesh Renderer
            Mesh<VertexP1N1T1B1UV>* particleQuadMesh;
            Material particleMaterial;

    };
}

#endif // __PARTICLE_EMITTER__

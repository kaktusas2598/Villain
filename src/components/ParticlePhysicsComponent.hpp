#pragma once

#include "Engine.hpp"
#include "Logger.hpp"
#include "NodeComponent.hpp"
#include "physics/Particle.hpp"
#include "physics/ParticleForceGenerator.hpp"
#include "rendering/Mesh.hpp"
#include "rendering/MeshUtils.hpp"

namespace Villain {

    // Represents some kind of physical particle system or device
    class ParticlePhysicsComponent : public NodeComponent {
        public:
            ParticlePhysicsComponent(bool kinematic = false): isKinematic(kinematic) {}

            virtual void render(Shader& shader, RenderingEngine& renderingEngine, Camera& camera);
            virtual void handleInput(float deltaTime);

            unsigned addParticle(Particle* particle);

            void addForceGenerator(ParticleForceGenerator* generator, const std::vector<size_t>& indices) {
                forceGenerators.push_back(generator);
                for (size_t index: indices)
                    getParticleWorld()->getForceRegistry().add(particles[index], generator);
            }

            void addContactGenerator(ParticleContactGenerator* generator) {
                contactGenerators.push_back(generator);
                getParticleWorld()->getContactGenerators().push_back(generator);
            }

            ParticleWorld* getParticleWorld() { return getParent()->getEngine()->getParticleWorld(); }
            std::vector<Particle*>& getParticles() { return particles; }

        private:
            std::vector<Particle*> particles;
            std::vector<glm::mat4> instanceMatrices;

            std::vector<ParticleForceGenerator*> forceGenerators;
            std::vector<ParticleContactGenerator*> contactGenerators;

            // Inspired by Mesh Renderer
            Mesh<VertexP1N1T1B1UV>* particleQuadMesh = nullptr;
            Material particleMaterial;

            bool isKinematic = false; //< Can be controlled via events
    };
}

#include "ParticlePhysicsComponent.hpp"

#include "glm/gtx/string_cast.hpp"
namespace Villain {

    ParticlePhysicsComponent::ParticlePhysicsComponent(bool kinematic)
        : isKinematic(kinematic) {
            VILLAIN_SET_COMPONENT_ID(ParticlePhysicsComponent);
        }

    void ParticlePhysicsComponent::render(Shader& shader, RenderingEngine& renderingEngine, Camera& camera) {
        instanceMatrices.resize(particles.size());
        for (int i = 0; i < particles.size(); ++i) {
            // Create a translation matrix based on the particle's position
            //VILLAIN_INFO("Position {}", glm::to_string(particles[i]->getPosition()));
            glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), particles[i]->getPosition());
            instanceMatrices[i] = translationMatrix;
        }

        particleQuadMesh->updateInstances(instanceMatrices);

        Material material;
        shader.updateUniforms(*parent->getTransform(), renderingEngine, camera);
        particleQuadMesh->draw(shader, material);
    }

    void ParticlePhysicsComponent::handleInput(float deltaTime) {
        // TODO: move to new KinematicParticleController class
        if (isKinematic) {
            GetTransform()->setPos(particles[0]->getPosition());

            if (Input::Get()->isKeyDown(SDLK_w)) {
                particles[0]->addForce(GetTransform()->getForward() * particles[0]->getMass());
            }
            if (Input::Get()->isKeyDown(SDLK_s)) {
                particles[0]->addForce(GetTransform()->getForward() * -particles[0]->getMass());
            }
            if (Input::Get()->isKeyDown(SDLK_a)) {
                particles[0]->addForce(GetTransform()->getRight() * -particles[0]->getMass());
            }
            if (Input::Get()->isKeyDown(SDLK_d)) {
                particles[0]->addForce(GetTransform()->getRight() * particles[0]->getMass());
            }
            if (Input::Get()->isKeyDown(SDLK_SPACE)) {
                particles[0]->addForce(GetTransform()->getUp() * particles[0]->getMass());
            }
            if (Input::Get()->isKeyDown(SDLK_LSHIFT)) {
                particles[0]->addForce(GetTransform()->getUp() * -particles[0]->getMass());
            }
        }
    }

    unsigned ParticlePhysicsComponent::addParticle(Particle* particle) {
        particles.push_back(particle);
        getParticleWorld()->getParticles().push_back(particle);

        if (particleQuadMesh == nullptr && particles.size() > 0) {
            // If we have at least one particle in the system, create a mesh with 1 instance
            std::vector<VertexP1N1T1B1UV> vertices;
            std::vector<unsigned int> indices;
            MeshUtils<VertexP1N1T1B1UV>::addSphere(&vertices, &indices, 1.0f, glm::vec3(0.0f));
            particleQuadMesh = new Mesh<VertexP1N1T1B1UV>(vertices, indices);

            // Check if this system is kinematic, which means it can be controlled via events with keyboard etc.
            //if (isKinematic) {
                //KinematicBodyListener* listener = new KinematicBodyListener();
                //listener->body = particles[0];
                //getParent()->getEngine()->getEventDispatcher()->registerListener(listener);
            //}

        }

        // Return index of newly added particle
        return particles.size() - 1;
    }
}

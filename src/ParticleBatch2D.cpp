#include "ParticleBatch2D.hpp"

namespace Villain {

    ParticleBatch2D::ParticleBatch2D() {}

    ParticleBatch2D::~ParticleBatch2D() {
        delete[] particles;
    }

    void ParticleBatch2D::init(
            int maxP,
            float decay,
            Texture* t,
            std::function<void(Particle2D&, float)> update
            ) {
        maxParticles = maxP;
        texture = t;
        decayRate = decay;
        particles = new Particle2D[maxParticles];
        updateFunc = update;
    }

    void ParticleBatch2D::update(float deltaTime) {
        for (int i = 0; i < maxParticles; i++) {
            if (particles[i].life > 0.0f) {
                updateFunc(particles[i], deltaTime);
                particles[i].life -= decayRate * deltaTime;
            }
        }
    }

    void ParticleBatch2D::draw(SpriteBatch* batch) {
        for (int i = 0; i < maxParticles; i++) {
            // Check if particle is alive/active
            auto& p = particles[i];
            if (p.life > 0.0f) {
                glm::vec4 destRect(p.position.x, p.position.y, p.width, p.width);
                // NOTE:Assume we dont use sprite atlas here, also refactor depth
                glm::vec4 uvRect(0.0f, 0.0f, 1.0f, 1.0f);
                batch->draw(destRect, uvRect, texture->getID(), 0.5f, p.color);
            }
        }
    }

    void ParticleBatch2D::addParticle(
            const glm::vec2& pos, const glm::vec2& vel, const glm::vec4& color, float width) {
        int pIndex = findFreeParticle();
        auto& p = particles[pIndex];

        p.life = 1.0f;
        p.position = pos;
        p.velocity = vel;
        p.color = color;
        p.width = width;
    }

    int ParticleBatch2D::findFreeParticle() {
        for (int i = lastFreeParticle; i < maxParticles; i++) {
            if (particles[i].life <= 0.0f) {
                lastFreeParticle = i;
                return i;
            }
        }

        // Wrap around and loop from other side
        for (int i = 0; i < lastFreeParticle; i++) {
            if (particles[i].life <= 0.0f) {
                lastFreeParticle = i;
                return i;
            }
        }

        // No free particles, override the first one
        return 0;
    }
}

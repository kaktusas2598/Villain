#include "ParticleEngine2D.hpp"

#include "ParticleBatch2D.hpp"
#include "rendering/SpriteBatch.hpp"

namespace Villain {

    ParticleEngine2D::ParticleEngine2D() {}

    ParticleEngine2D::~ParticleEngine2D() {
        for (auto& b : batches) {
            delete b;
        }
    }

    void ParticleEngine2D::addParticleBatch(ParticleBatch2D* batch) {
        batches.push_back(batch);
    }

    void ParticleEngine2D::update(float deltaTime) {
        for (auto& b : batches) {
            b->update(deltaTime);
        }
    }

    void ParticleEngine2D::draw(SpriteBatch* batch) {
        for (auto& b : batches) {
            batch->begin();
            b->draw(batch);
            batch->end();
            batch->renderBatch();
        }
    }
}

#pragma once

#include <vector>

namespace Villain {

    class ParticleBatch2D;
    class SpriteBatch;

    // Cache friendly 2D Particle Engine rendering in batches
    // Responsible for managing particle emitters and deallocating memory
    class ParticleEngine2D {
        public:
            ParticleEngine2D();
            ~ParticleEngine2D();

            void addParticleBatch(ParticleBatch2D* batch);

            void update(float deltaTime);
            void draw(SpriteBatch* batch);
        private:
            std::vector<ParticleBatch2D*> batches;

    };
}

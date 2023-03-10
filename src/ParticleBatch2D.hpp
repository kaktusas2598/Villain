#ifndef __PARTICLE_BATCH_2D__
#define __PARTICLE_BATCH_2D__

#include "SpriteBatch.hpp"

namespace Villain {

    // 2D Particle model
    class Particle2D {
        public:
            friend class ParticleBatch2D;

            void update(float deltaTime);
        private:
            // C++11 header initialization
            glm::vec2 position = glm::vec2(0.0f);
            glm::vec2 velocity = glm::vec2(0.0f);
            glm::vec4 color;
            float life = 0.0f;
            float width = 0.0f; //<<< Assume all particles are square
    };

    // 2D Particle batch/emitter
    class ParticleBatch2D {
        public:
            ParticleBatch2D();
            ~ParticleBatch2D();

            /**
             * @brief Initialise particle rendering batch
             *
             * @param maxP Max number of particles in a batch
             * @param decay Particles decay rate
             * @param t Pointer to texture resource
             */
            void init(int maxP, float decay, Texture* t);

            void update(float deltaTime);
            void draw(SpriteBatch* batch);

            /**
             * @brief Add a new particle to a rendering batch
             *
             * @param pos Particle position
             * @param vel Particle velocity
             * @param color Particle color
             * @param width Particle width/height
             */
            void addParticle(const glm::vec2& pos, const glm::vec2& vel, const glm::vec4& color, float width);
        private:
            int findFreeParticle();

            // Global batch properties for all particles
            float decayRate = 0.1f; //<<< How quickly particles disappear
            Particle2D* particles = nullptr;
            int maxParticles = 0; //<<< Maximum number of particles in a batch
            int lastFreeParticle = 0; //<<< Store last free particle index to save up on performance
            Texture* texture;
    };
}

#endif // __PARTICLE_BATCH_2D__

#pragma once

#include "Shader.hpp"
#include "Texture.hpp"
#include "VertexArray.hpp"

#include <memory>

namespace Villain {

    class SkyDome {
        public:
            SkyDome(int numPitchStripes, int numHeadingStripes, float radius, Texture* texture = nullptr);
            ~SkyDome() {}

            void render(Camera* camera);

        private:
            // Probably should go to some math utils static helper
            glm::vec3 sphericalToCartesian(float radius, float inclinationDegrees, float azimuthDegrees);

            void populateBuffers(int numRows, int numCols, int radius);

            std::unique_ptr<VertexArray> skydomeVao;
            std::unique_ptr<VertexBuffer> skydomeVbo;
            int numVertices = 0;

            Shader* skyDomeShader;
            Texture* skyDomeTexture;
    };
}

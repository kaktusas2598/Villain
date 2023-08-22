#pragma once

#include "SkyBox.hpp"

namespace Villain {

    /// ATM extends Skybox because it shares cubemap mesh, but might change in the future
    class HDRMap : public SkyBox {
        public:
            HDRMap(const std::string& hdrTexturePath);

            // Must be rendered LAST every frame
            void render(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix);

        private:
            Shader* equirectangularToCubeMap;
            Texture* hdrTexture;
            unsigned int envCubemap;
    };
}

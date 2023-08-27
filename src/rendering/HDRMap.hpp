#pragma once

#include "SkyBox.hpp"

namespace Villain {

    /// ATM extends Skybox because it shares cubemap mesh, but might change in the future
    class HDRMap : public SkyBox {
        public:
            HDRMap(const std::string& hdrTexturePath);

            /// Must be rendered LAST every frame
            void render(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, float exposure = 1.0);

            unsigned getHDRTextureId() const { return hdrTexture->getID(); }
            unsigned getEnvironmentMapId() const { return envCubemap; }
            unsigned getIrradianceMapId() const { return irradianceMap; }

        private:
            Shader* equirectangularToCubeMap; ///< Converts equirectangular texture to cubemap
            Shader* irradianceConvolutionShader; ///< Takes in cubemap texture and convolutes it producing irradiance map
            Shader* prefilterConvolutionShader; ///< Produces pre filtered convoluted map for specular IBL calculations
            Shader* brdfIntegrationShader; ///< Produces convoluted BRDF part of the split sum integral approach for specular IBL
            Texture* hdrTexture; ///< Equirectangular HDR environmental map image texture
            unsigned int envCubemap; ///< HDR texture converted to cubemap
            unsigned int irradianceMap; ///< Convoluted hdr cubemap used as scene's irradiance map for diffuse IBL
            unsigned int prefilteredMap; ///< Pre-computed environment convolution map used for specular IBL
            unsigned int brdfLUTTexture; ///< 2D lookup texture (LUT) - BRDF integration map used for specular IBL
    };
}

#include "PBRMaterial.hpp"

#include "RendereringEngine.hpp"

namespace Villain {

    void PBRMaterial::updateUniforms(Shader& shader)  {
        // Tell shader to switch light calculations from blinn-phong to PBR model
        shader.setUniform1i("usePBR", true);

        // Base color map
        if (albedoMap == nullptr) {
            shader.setUniform1i("material.useAlbedoMap", 0);
        } else {
            shader.setUniform1i("material.useAlbedoMap", 1);
            albedoMap->bind(RenderingEngine::getSamplerSlot("albedo"));
            shader.setUniform1i("material.texture_albedo", RenderingEngine::getSamplerSlot("albedo"));
        }
        // Normal/bump map
        if (normalMap == nullptr) {
            shader.setUniform1i("material.useNormalMap", 0);
        } else {
            shader.setUniform1i("material.useNormalMap", 1);
            normalMap->bind(RenderingEngine::getSamplerSlot("normal"));
            shader.setUniform1i("material.texture_normal", RenderingEngine::getSamplerSlot("normal"));
        }

        // Metallic map
        if (metallicMap == nullptr) {
            shader.setUniform1i("material.useMetallicMap", 0);
        } else {
            shader.setUniform1i("material.useMetallicMap", 1);
            metallicMap->bind(RenderingEngine::getSamplerSlot("metallic"));
            shader.setUniform1i("material.texture_metallic", RenderingEngine::getSamplerSlot("metallic"));
        }

        // Roughness map
        if (roughnessMap == nullptr) {
            shader.setUniform1i("material.useRoughnessMap", 0);
        } else {
            shader.setUniform1i("material.useRoughnessMap", 1);
            roughnessMap->bind(RenderingEngine::getSamplerSlot("roughness"));
            shader.setUniform1i("material.texture_roughness", RenderingEngine::getSamplerSlot("roughness"));
        }

        // Ambient oclussion map
        if (aoMap == nullptr) {
            shader.setUniform1i("material.useAOMap", 0);
        } else {
            shader.setUniform1i("material.useAOMap", 1);
            aoMap->bind(RenderingEngine::getSamplerSlot("ao"));
            shader.setUniform1i("material.texture_ao", RenderingEngine::getSamplerSlot("ao"));
        }

        // TODO: Parallax displacement map
    }
}

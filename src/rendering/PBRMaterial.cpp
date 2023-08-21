#include "PBRMaterial.hpp"

#include "RendereringEngine.hpp"

namespace Villain {

    void PBRMaterial::updateUniforms(Shader& shader)  {
        // Tell shader to switch light calculations from blinn-phong to PBR model
        shader.setUniform1i("usePBR", true);

        shader.setUniformVec3("pbrMaterial.albedo", albedo);
        shader.setUniform1f("pbrMaterial.roughness", roughness);
        shader.setUniform1f("pbrMaterial.metallic", metallic);
        shader.setUniform1f("pbrMaterial.ao", ao);

        // Base color map
        if (albedoMap == nullptr) {
            shader.setUniform1i("pbrMaterial.useAlbedoMap", 0);
        } else {
            shader.setUniform1i("pbrMaterial.useAlbedoMap", 1);
            albedoMap->bind(RenderingEngine::getSamplerSlot("albedo"));
            shader.setUniform1i("pbrMaterial.texture_albedo", RenderingEngine::getSamplerSlot("albedo"));
        }
        // Normal/bump map
        if (normalMap == nullptr) {
            shader.setUniform1i("pbrMaterial.useNormalMap", 0);
        } else {
            shader.setUniform1i("pbrMaterial.useNormalMap", 1);
            normalMap->bind(RenderingEngine::getSamplerSlot("normal"));
            shader.setUniform1i("pbrMaterial.texture_normal", RenderingEngine::getSamplerSlot("normal"));
        }
        // Emissive map
        if (emissionMap == nullptr) {
            shader.setUniform1i("pbrMaterial.useEmissionMap", 0);
        } else {
            shader.setUniform1i("pbrMaterial.useEmissionMap", 1);
            emissionMap->bind(RenderingEngine::getSamplerSlot("emission"));
            shader.setUniform1i("pbrMaterial.texture_emission", RenderingEngine::getSamplerSlot("emission"));
        }

        // Metallic map
        if (metallicMap == nullptr) {
            shader.setUniform1i("pbrMaterial.useMetallicMap", 0);
        } else {
            shader.setUniform1i("pbrMaterial.useMetallicMap", 1);
            metallicMap->bind(RenderingEngine::getSamplerSlot("metallic"));
            shader.setUniform1i("pbrMaterial.texture_metallic", RenderingEngine::getSamplerSlot("metallic"));
        }

        // Roughness map
        if (roughnessMap == nullptr) {
            shader.setUniform1i("pbrMaterial.useRoughnessMap", 0);
        } else {
            shader.setUniform1i("pbrMaterial.useRoughnessMap", 1);
            roughnessMap->bind(RenderingEngine::getSamplerSlot("roughness"));
            shader.setUniform1i("pbrMaterial.texture_roughness", RenderingEngine::getSamplerSlot("roughness"));
        }

        // Ambient oclussion map
        if (aoMap == nullptr) {
            shader.setUniform1i("pbrMaterial.useAOMap", 0);
        } else {
            shader.setUniform1i("pbrMaterial.useAOMap", 1);
            aoMap->bind(RenderingEngine::getSamplerSlot("ao"));
            shader.setUniform1i("pbrMaterial.texture_ao", RenderingEngine::getSamplerSlot("ao"));
        }

        // TODO: Parallax displacement map
    }
}

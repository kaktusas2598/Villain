#include "Material.hpp"

#include "RendereringEngine.hpp"

namespace Villain {

    void Material::updateUniforms(Shader& shader) {
        shader.setUniform1i("usePBR", false);

        // Base color/diffuse map
        if (diffuseMap == nullptr) {
            shader.setUniform1i("material.useDiffuseMap", 0);
        } else {
            shader.setUniform1i("material.useDiffuseMap", 1);
            diffuseMap->bind(RenderingEngine::getSamplerSlot("diffuse"));
            shader.setUniform1i("material.texture_diffuse", RenderingEngine::getSamplerSlot("diffuse"));
        }
        // Specular map
        if (specularMap == nullptr) {
            shader.setUniform1i("material.useSpecularMap", 0);
        } else {
            shader.setUniform1i("material.useSpecularMap", 1);
            specularMap->bind(RenderingEngine::getSamplerSlot("specular"));
            shader.setUniform1i("material.texture_specular", RenderingEngine::getSamplerSlot("specular"));
        }
        // Normal/bump map
        if (normalMap == nullptr) {
            shader.setUniform1i("material.useNormalMap", 0);
        } else {
            shader.setUniform1i("material.useNormalMap", 1);
            normalMap->bind(RenderingEngine::getSamplerSlot("normal"));
            shader.setUniform1i("material.texture_normal", RenderingEngine::getSamplerSlot("normal"));
        }
        // Parallax displacement map
        if (dispMap == nullptr) {
            shader.setUniform1i("material.useDispMap", 0);
        } else {
            shader.setUniform1i("material.useDispMap", 1);
            dispMap->bind(RenderingEngine::getSamplerSlot("disp"));
            shader.setUniform1i("material.texture_disp", RenderingEngine::getSamplerSlot("disp"));
            shader.setUniform1f("material.dispMapScale", dispMapScale);
            shader.setUniform1f("material.dispMapBias", dispMapBias);
        }

        shader.setUniform1f("material.shininess", specularFactor);
        shader.setUniformVec4("material.ambientColor", ambientColor);
        shader.setUniformVec4("material.diffuseColor", diffuseColor);
        shader.setUniformVec4("material.specularColor", specularColor);
    }
}

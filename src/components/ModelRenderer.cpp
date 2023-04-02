#include "ModelRenderer.hpp"

#include "Material.hpp"
#include "ResourceManager.hpp"

namespace Villain {

    ModelRenderer::ModelRenderer(const std::string& fileName) {
        model = new Model(fileName.c_str());
    }

    ModelRenderer::~ModelRenderer() {
        delete model;
    }

    // TODO:
    void ModelRenderer::render(
            Shader& shader,
            RenderingEngine& renderingEngine,
            Camera& camera
            ) {
        shader.bind();
        // TODO: need different method to update uniforms, stuff like matrixes can be updated the same for all meshes
        // materials are different per mesh
        Material material;
        shader.updateUniforms(*parent->getTransform(), material, renderingEngine, camera);
        //model->draw(shader);
        for (auto& mesh: model->getMeshes()) {
            // Draw mesh using it's own material
            mesh.draw(shader, model->getMaterials()[mesh.getMaterialName()]);
        }
    }
}

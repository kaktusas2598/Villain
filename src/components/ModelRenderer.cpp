#include "ModelRenderer.hpp"

#include "rendering/Material.hpp"
#include "rendering/RendereringEngine.hpp"
#include "ResourceManager.hpp"

namespace Villain {

    ModelRenderer::ModelRenderer(const std::string& fileName) {
        model = new Model(fileName.c_str());
    }

    ModelRenderer::~ModelRenderer() {
        delete model;
    }

    void ModelRenderer::render(
            Shader& shader,
            RenderingEngine& renderingEngine,
            Camera& camera
            ) {
		//unsigned int display = 0, total = 0;
		const Frustum camFrustum = camera.getFrustum();

        shader.bind();
        // TODO: need different method to update uniforms, stuff like matrixes can be updated the same for all meshes
        // materials are different per mesh
        Material material;
        shader.updateUniforms(*parent->getTransform(), material, renderingEngine, camera);
        //model->draw(shader);
        for (auto& mesh: model->getMeshes()) {
            if (renderingEngine.isFrustumCullingEnabled()) {
                if (mesh.getBoundingVolume()->isOnFrustum(camFrustum, *GetTransform())) {
                    // Draw mesh using it's own material
                    mesh.draw(shader, model->getMaterials()[mesh.getMaterialName()]);
                    //display++;
                }
                //total++;
            } else {
                mesh.draw(shader, model->getMaterials()[mesh.getMaterialName()]);
            }
        }
        //std::cout << "Total meshes: " << total << ", Visible meshes: " << display << "\n";
    }
}

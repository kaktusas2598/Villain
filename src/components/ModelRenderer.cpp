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

        shader.bind();
        // TODO: need different method to update uniforms, stuff like matrixes can be updated the same for all meshes
        // materials are different per mesh
        Material material;
        shader.updateUniforms(*parent->getTransform(), material, renderingEngine, camera);
        shader.setUniform1ui("objectIndex", parent->getID());
        if (renderingEngine.getSelectedNodeID() != 0 && renderingEngine.getSelectedNodeID() == parent->getID()) {
            parent->setSelected(true);
        } else {
            parent->setSelected(false);
        }
        shader.setUniform1i("selected", parent->isSelected());
        //model->draw(shader);

        int i = 0;
        for (auto& mesh: model->getMeshes()) {
            if (renderingEngine.isFrustumCullingEnabled()) {
                const Frustum camFrustum = camera.getFrustum();
                if (mesh.getBoundingVolume()->isOnFrustum(camFrustum, *GetTransform())) {
                    shader.setUniform1ui("drawIndex", i);
                    // Draw mesh using it's own material
                    mesh.draw(shader, model->getMaterials()[mesh.getMaterialName()]);
                    //display++;
                }
                //total++;
            } else {
                mesh.draw(shader, model->getMaterials()[mesh.getMaterialName()]);
            }
            i++;
        }
        //std::cout << "Total meshes: " << total << ", Visible meshes: " << display << "\n";
    }
}

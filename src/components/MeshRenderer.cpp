#include "MeshRenderer.hpp"

#include "components/Light.hpp"
#include "rendering/Material.hpp"
#include "rendering/RendereringEngine.hpp"
#include "ResourceManager.hpp"

namespace Villain {

    template <class VertexType>
        MeshRenderer<VertexType>::MeshRenderer(Mesh<VertexType>* mesh, Material material) {
            this->mesh = mesh;
            this->material = material;
        }

    template <class VertexType>
        void MeshRenderer<VertexType>::render(
                Shader& shader,
                RenderingEngine& renderingEngine,
                Camera& camera
                ) {

            shader.bind();
            // NOTE: should implenent mesh batch renderer
            // TODO: Add new updateUniforms() method without a matierla, because meshes set their own materials for each mesh
            shader.updateUniforms(*parent->getTransform(), this->material, renderingEngine, camera);
            shader.setUniform1ui("objectIndex", parent->getID());
            shader.setUniform1ui("drawIndex", 0);
            if (renderingEngine.getSelectedNodeID() != 0 && renderingEngine.getSelectedNodeID() == parent->getID()) {
                parent->setSelected(true);
            } else {
                parent->setSelected(false);
            }
            shader.setUniform1i("selected", parent->isSelected());

            // NOTE: For now instanced meshes are not culled by camera's frustum
            if (renderingEngine.isFrustumCullingEnabled() && !mesh->isInstanced()) {
                const Frustum camFrustum = camera.getFrustum();
                if (mesh->getBoundingVolume()->isOnFrustum(camFrustum, *GetTransform())) {
                    // Draw mesh using it's own material
                    mesh->draw(shader, this->material);
                    //std::cout << "Mesh in frustum\n";
                } else {
                    //std::cout << "Mesh culled by frustum\n";
                }
            } else {
                mesh->draw(shader, this->material);
            }
        }

    // Explicit instantiation of template specialisations to avoid linker errors, alternativaly and even better
    // template should be defined in header file only
    template MeshRenderer<VertexP1>::MeshRenderer(Mesh<VertexP1>* mesh, Material material);
    template MeshRenderer<VertexP1UV>::MeshRenderer(Mesh<VertexP1UV>* mesh, Material material);
    template MeshRenderer<VertexP1N1>::MeshRenderer(Mesh<VertexP1N1>* mesh, Material material);
    template MeshRenderer<VertexP1N1UV>::MeshRenderer(Mesh<VertexP1N1UV>* mesh, Material material);
    template MeshRenderer<VertexP1C1UV>::MeshRenderer(Mesh<VertexP1C1UV>* mesh, Material material);
    template MeshRenderer<VertexP1N1C1UV>::MeshRenderer(Mesh<VertexP1N1C1UV>* mesh, Material material);
    template MeshRenderer<VertexP1N1T1B1UV>::MeshRenderer(Mesh<VertexP1N1T1B1UV>* mesh, Material material);

    template void MeshRenderer<VertexP1>::render(Shader& shader, RenderingEngine& renderingEngine, Camera& camera);
    template void MeshRenderer<VertexP1UV>::render(Shader& shader, RenderingEngine& renderingEngine, Camera& camera);
    template void MeshRenderer<VertexP1N1>::render(Shader& shader, RenderingEngine& renderingEngine, Camera& camera);
    template void MeshRenderer<VertexP1N1UV>::render(Shader& shader, RenderingEngine& renderingEngine, Camera& camera);
    template void MeshRenderer<VertexP1C1UV>::render(Shader& shader, RenderingEngine& renderingEngine, Camera& camera);
    template void MeshRenderer<VertexP1N1C1UV>::render(Shader& shader, RenderingEngine& renderingEngine, Camera& camera);
    template void MeshRenderer<VertexP1N1T1B1UV>::render(Shader& shader, RenderingEngine& renderingEngine, Camera& camera);
}

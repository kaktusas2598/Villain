#include "MeshRenderer.hpp"

#include "Material.hpp"
#include "ResourceManager.hpp"

template <class VertexType>
MeshRenderer<VertexType>::MeshRenderer(Villain::Mesh<VertexType>* mesh, Villain::Material material) {
    this->mesh = mesh;
    this->material = material;
}

template <class VertexType>
void MeshRenderer<VertexType>::render(
            Villain::Shader& shader,
            Villain::RenderingEngine& renderingEngine,
            Villain::Camera& camera
            ) {
    shader.bind();
    // TODO: Update uniforms :: matrices, materials, lights, camera etc.
    // TODO: pass to rendering engine (maybe should implenent mesh batch renderer)
    shader.updateUniforms(*parent->getTransform(), this->material, renderingEngine, camera);

    mesh->draw(shader);
}

// Explicit instantiation of template specialisations to avoid linker errors, alternativaly and even better
// template should be defined in header file only
template MeshRenderer<VertexP1>::MeshRenderer(Villain::Mesh<VertexP1>* mesh, Villain::Material material);
template MeshRenderer<VertexP1UV>::MeshRenderer(Villain::Mesh<VertexP1UV>* mesh, Villain::Material material);
template MeshRenderer<VertexP1N1>::MeshRenderer(Villain::Mesh<VertexP1N1>* mesh, Villain::Material material);
template MeshRenderer<VertexP1N1UV>::MeshRenderer(Villain::Mesh<VertexP1N1UV>* mesh, Villain::Material material);
template MeshRenderer<VertexP1C1UV>::MeshRenderer(Villain::Mesh<VertexP1C1UV>* mesh, Villain::Material material);
template MeshRenderer<VertexP1N1C1UV>::MeshRenderer(Villain::Mesh<VertexP1N1C1UV>* mesh, Villain::Material material);

template void MeshRenderer<VertexP1>::render(Villain::Shader& shader, Villain::RenderingEngine& renderingEngine, Villain::Camera& camera);
template void MeshRenderer<VertexP1UV>::render(Villain::Shader& shader, Villain::RenderingEngine& renderingEngine, Villain::Camera& camera);
template void MeshRenderer<VertexP1N1>::render(Villain::Shader& shader, Villain::RenderingEngine& renderingEngine, Villain::Camera& camera);
template void MeshRenderer<VertexP1N1UV>::render(Villain::Shader& shader, Villain::RenderingEngine& renderingEngine, Villain::Camera& camera);
template void MeshRenderer<VertexP1C1UV>::render(Villain::Shader& shader, Villain::RenderingEngine& renderingEngine, Villain::Camera& camera);
template void MeshRenderer<VertexP1N1C1UV>::render(Villain::Shader& shader, Villain::RenderingEngine& renderingEngine, Villain::Camera& camera);



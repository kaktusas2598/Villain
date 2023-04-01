#ifndef __MESH_RENDERER__
#define __MESH_RENDERER__

#include "../Material.hpp"
#include "../Mesh.hpp"
#include "../NodeComponent.hpp"

// TODO: will need to be template class to be able to take in any type of vertex
template <class VertexType>
class MeshRenderer : public Villain::NodeComponent {
    public:
        MeshRenderer(Villain::Mesh<VertexType>* mesh, Villain::Material material);
        virtual void render(
            Villain::Shader& shader,
            Villain::RenderingEngine& renderingEngine,
            Villain::Camera& camera
            ) override;
    private:
        Villain::Mesh<VertexType>* mesh;
        Villain::Material material;
};

#endif // __MESH_RENDERER__

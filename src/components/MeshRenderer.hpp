#ifndef __MESH_RENDERER__
#define __MESH_RENDERER__

#include "../Material.hpp"
#include "../Mesh.hpp"
#include "../NodeComponent.hpp"

namespace Villain {

    template <class VertexType>
    class MeshRenderer : public NodeComponent {
        public:
            MeshRenderer(Mesh<VertexType>* mesh, Material material);
            virtual void render(
                Shader& shader,
                RenderingEngine& renderingEngine,
                Camera& camera
                ) override;
        protected:
            Mesh<VertexType>* mesh;
            Material material;
    };
}

#endif // __MESH_RENDERER__

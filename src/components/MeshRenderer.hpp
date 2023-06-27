#ifndef __MESH_RENDERER__
#define __MESH_RENDERER__

#include "../rendering/Material.hpp"
#include "../rendering/Mesh.hpp"
#include "../NodeComponent.hpp"

namespace Villain {

    template <class VertexType>
    class MeshRenderer : public NodeComponent {
        public:
            DECLARE_COMPONENT_TYPE(MeshRenderer);

            MeshRenderer(Mesh<VertexType>* mesh, Material material);
            virtual void render(
                Shader& shader,
                RenderingEngine& renderingEngine,
                Camera& camera
                ) override;

            Mesh<VertexType>* getMesh() const { return mesh; }
            Material& getMaterial() { return material; }

            void setMesh(Mesh<VertexType>* m) { mesh = m; }
        protected:
            Mesh<VertexType>* mesh;
            Material material;
    };
}

#endif // __MESH_RENDERER__

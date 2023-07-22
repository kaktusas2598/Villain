#pragma once

#include "../rendering/Material.hpp"
#include "../rendering/Mesh.hpp"
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

            Mesh<VertexType>* getMesh() const { return mesh; }
            Material& getMaterial() { return material; }

            void setMesh(Mesh<VertexType>* m) { mesh = m; }
        protected:
            Mesh<VertexType>* mesh;
            Material material;
    };
}

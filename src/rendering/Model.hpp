#ifndef __Model__
#define __Model__

#include "Material.hpp"
#include "Mesh.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <map>

namespace Villain {

    // Used for abstracting Model data
    // and loading using assimp loading interface
    // NOTE: We are using Vertex type which contains tangent space, but it will be useless
    // if model does not contain normal maps
    class Model {
        public:
            Model (const char* path) { loadModel(path); }
            void draw(Shader& shader);

            std::map<std::string, Material>& getMaterials() { return materials; }
            std::vector<Mesh<VertexP1N1T1B1UV>>& getMeshes() { return meshes; }
        private:
            std::vector<Mesh<VertexP1N1T1B1UV>> meshes;
            std::map<std::string, Material> materials;
            std::string directory;

            void loadModel(std::string path);
            void processNode(aiNode* node, const aiScene* scene);
            Mesh<VertexP1N1T1B1UV> processMesh(aiMesh* mesh, const aiScene* scene);
            std::vector<Texture*>* loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
    };
}

#endif // __Model__
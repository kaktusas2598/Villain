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
    class Model {
        public:
            Model (const char* path) { loadModel(path); }
            void draw(Shader& shader);

            std::map<std::string, Material>& getMaterials() { return materials; }
            std::vector<Mesh<VertexP1N1UV>>& getMeshes() { return meshes; }
        private:
            std::vector<Mesh<VertexP1N1UV>> meshes;
            std::map<std::string, Material> materials;
            std::string directory;

            void loadModel(std::string path);
            void processNode(aiNode* node, const aiScene* scene);
            Mesh<VertexP1N1UV> processMesh(aiMesh* mesh, const aiScene* scene);
            std::vector<Texture*>* loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
    };
}

#endif // __Model__

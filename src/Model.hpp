#ifndef __Model__
#define __Model__

#include "Mesh.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Villain {

    // Used for abstracting Model data
    // and loading using assimp loading interface
    class Model {
        public:
            Model (const char* path) { loadModel(path); }
            void draw(Shader& shader);
        private:
            std::vector<Mesh> meshes;
            std::string directory;

            void loadModel(std::string path);
            void processNode(aiNode* node, const aiScene* scene);
            Mesh processMesh(aiMesh* mesh, const aiScene* scene);
            std::vector<Texture*>* loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
    };
}

#endif // __Model__

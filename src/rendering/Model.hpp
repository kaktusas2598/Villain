#ifndef __Model__
#define __Model__

#include "Material.hpp"
#include "Mesh.hpp"
#include "rendering/animation/BoneInfo.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <map>

namespace Villain {

    class Animation;
    class Animator;

    // Used for abstracting Model data
    // and loading using assimp loading interface
    // NOTE: We are using Vertex type which contains tangent space, but it will be useless
    // if model does not contain normal maps
    class Model {
        public:
            Model (const char* path, unsigned instances = 1, std::vector<glm::mat4> instanceTransforms = {}){
                numInstances = instances;
                instanceMatrix = instanceTransforms;
                loadModel(path);
            }
            ~Model();

            void draw(Shader& shader);

            std::map<std::string, Material>& getMaterials() { return materials; }
            std::vector<Mesh<VertexP1N1T1B1UV>>& getMeshes() { return meshes; }

            std::string& getDirectory() { return directory; }
            std::string& getFilename() { return fileName; }
            auto& getBoneInfoMap() { return boneInfoMap; }
            int& getBoneCount() { return boneCounter; }
            int getDisplayedBoneIndex() const { return displayedBoneIndex; }
            void setDisplayedBoneIndex(int displayIndex) { displayedBoneIndex = displayIndex; }
            Animator* getAnimator() { return animator; }
            std::map<std::string, Animation*>& getAnimations() { return animationMap; }

        private:
            Assimp::Importer importer;
            const aiScene* scene = nullptr;

            std::vector<Mesh<VertexP1N1T1B1UV>> meshes;
            std::map<std::string, Material> materials;
            std::string directory;
            std::string fileName;

            // Instanced rendering properties
            unsigned int numInstances = 1; //<<< Set to more than 1 instance for instanced rendering
            std::vector<glm::mat4> instanceMatrix;
            std::unique_ptr<VertexBuffer> instanceVbo;

            void loadModel(const std::string& path);
            void processNode(aiNode* node, const aiScene* scene);
            Mesh<VertexP1N1T1B1UV> processMesh(aiMesh* mesh, const aiScene* scene);
            std::vector<Texture*>* loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);


            // Skeletal animation properties
            Animator* animator = nullptr;
            std::map<std::string, Animation*> animationMap;
            std::map<std::string, BoneInfo> boneInfoMap;
            int boneCounter = 0;
            int displayedBoneIndex = -1; //<<< For debugging bone weights


            void setVertexBoneData(VertexP1N1T1B1UV& vertex, int boneID, float weight);
            void resetVertexBoneData(VertexP1N1T1B1UV& vertex) {
                for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
                    vertex.BoneIDs[i] = -1;
                    vertex.Weights[i] = 0.0f;

                }
            }
            void extractBoneWeightForVertices(std::vector<VertexP1N1T1B1UV>& vertices, aiMesh* mesh, const aiScene* scene);
            void processAnimations(const aiScene* scene);
    };
}

#endif // __Model__

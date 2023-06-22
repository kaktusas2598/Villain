#include "Model.hpp"

#include "Logger.hpp"
#include "ResourceManager.hpp"
#include "rendering/animation/Animation.hpp"
#include "rendering/animation/Animator.hpp"
#include "rendering/AssimpUtils.hpp"
#include "rendering/RendereringEngine.hpp"

#include <sstream>

namespace Villain {

    Model::~Model() {
        if (animator) delete animator;

        for (auto it = animationMap.begin(); it != animationMap.end(); ++it) {
            Animation* ptr = it->second;
            delete ptr;
            animationMap.erase(it);
        }
    }

    void Model::draw(Shader& shader) {
        for (auto& mesh: meshes) {
            // Draw mesh using it's own material
            mesh.draw(shader, materials[mesh.getMaterialName()]);
        }
    }

    void Model::loadModel(std::string path) {
        // While loading scene, tell assimp to make sure uv coords are flipped along y axis
        // and all primitives are triangles
        // Other useful options:
        // aiProcess_GenNormals
        // aiProcess_CalcTangentSpace - won't calculate if there are no normals
        // NOTE: 2023-04-05: While doing tests with sponza model, disabled aiProcess_FlipUVs and model is working fine now
        scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices);

        printf("------------------------\n");
        printf("Loading model from %s\n", path.c_str());

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::stringstream ss;
            ss << "ASSIMP ERROR: " << importer.GetErrorString();
            Logger::Instance()->error(ss.str().c_str());
            return;
        }
        directory = path.substr(0, path.find_last_of('/'));
        fileName = path.substr(path.find_last_of('/'), path.length());

        printf("Number of animations: %d, meshes: %d\n", scene->mNumAnimations, scene->mNumMeshes);

        processNode(scene->mRootNode, scene);
        // NOTE: Must be called after processing mesh! Not good design
        processAnimations(scene);
    }

    void Model::processNode(aiNode* node, const aiScene* scene) {
        // process any meshes
        for (unsigned int i = 0; i < node->mNumMeshes; i ++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            //printf("Processing Mesh %s, Vertices: %d Indices: %d Bones: %d\n", mesh->mName.C_Str(), mesh->mNumVertices, mesh->mNumFaces * 3, mesh->mNumBones);
            meshes.push_back(processMesh(mesh, scene));
        }
        // process children nodes
        for (unsigned int i = 0; i < node->mNumChildren; i ++) {
            processNode(node->mChildren[i], scene);
        }
    }

    Mesh<VertexP1N1T1B1UV> Model::processMesh(aiMesh* mesh, const aiScene* scene) {
        std::vector<VertexP1N1T1B1UV> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture*> textures;
        std::string matName = std::string();

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            VertexP1N1T1B1UV vertex;

            resetVertexBoneData(vertex);

            vertex.Position = AssimpUtils::aiVector3ToGLM(mesh->mVertices[i]);
            vertex.Normal = mesh->HasNormals() ? AssimpUtils::aiVector3ToGLM(mesh->mNormals[i]) : glm::vec3(0.0f);

            vertex.Tangent = mesh->HasTangentsAndBitangents() ? AssimpUtils::aiVector3ToGLM(mesh->mTangents[i]) : glm::vec3(0.0f);
            vertex.BiTangent = mesh->HasTangentsAndBitangents() ? AssimpUtils::aiVector3ToGLM(mesh->mBitangents[i]) : glm::vec3(0.0f);

            // Check if texture coords are set, assimp supports up to 8 tex coords for each vertex
            if (mesh->mTextureCoords[0]) {
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.UV = vec;
            } else {
                vertex.UV = glm::vec2(0.0f, 0.0f);
            }

            vertices.push_back(vertex);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        if (mesh->HasBones()) {
            extractBoneWeightForVertices(vertices, mesh, scene);

            // Double check weights for each vertex and normalize if needed, this helps solve animation
            // issues on some models
            for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
                float sum = 0;
                for (int j = 0; j < MAX_BONE_INFLUENCE; j++) {
                    if (vertices[i].Weights[j] != -1)
                        sum += vertices[i].Weights[j];
                }
                if (sum < 1) {
                    // Normalizing weights
                    vertices[i].Weights[0] /= sum;
                    vertices[i].Weights[1] /= sum;
                    vertices[i].Weights[2] /= sum;
                    vertices[i].Weights[3] /= sum;
                }
            }
        }

        // Set diffuse color for entire mesh and if it has any materials, set if from there
        // including diffuse maps and other
        glm::vec4 diffuseColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        if (mesh->mMaterialIndex >= 0) {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

            aiColor4D diffuse;
            if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuse)) {
                diffuseColor = glm::vec4(diffuse.r, diffuse.g, diffuse.b, diffuse.a);
            }

            std::vector<Texture*>* diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps->begin(), diffuseMaps->end());
            std::vector<Texture*>* specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
            textures.insert(textures.end(), specularMaps->begin(), specularMaps->end());
            std::vector<Texture*>* normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal");
            textures.insert(textures.end(), normalMaps->begin(), normalMaps->end());

            matName = std::string(material->GetName().C_Str());
            Texture* diffuseMap = nullptr;
            Texture* specularMap = nullptr;
            Texture* normalMap = nullptr;
            if (!diffuseMaps->empty())
                diffuseMap = (*diffuseMaps)[0];
            if (!specularMaps->empty())
                specularMap = (*specularMaps)[0];
            if (!normalMaps->empty())
                normalMap = (*normalMaps)[0];
            // TODO: parallax displacement maps
            // TODO: stop hardcoding specularity factor(32) here
            Material mat(matName, diffuseMap, 32.0f, specularMap, normalMap);
            mat.setDiffuseColor(diffuseColor);
            materials[matName] = mat;
            return Mesh<VertexP1N1T1B1UV>(vertices, indices, matName, numInstances, instanceMatrix);
        }

        Logger::Instance()->warn("Mesh missing material information");
        return Mesh<VertexP1N1T1B1UV>(vertices, indices, numInstances, instanceMatrix);
    }

    std::vector<Texture*>* Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName) {
        // Maps used for light calculations (normal, speculars) will be in linear space and don't need to be gamma gamma corrected
        // or the lighting will look off
        bool gammaCorrected = (type == aiTextureType_DIFFUSE) && RenderingEngine::gammaCorrectionEnabled();
        std::vector<Texture*>* textures = new std::vector<Texture*>();
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
            aiString str;
            mat->GetTexture(type, i, &str);
            // NOTE: 2023-04-16, Started setting here GL_REPEAT explicitely which is
            // default wrapping mode anyway and this fixes issues with 3D models, so it will probably stay
            // atm default mode in engine is GL_CLAMP_TO_EDGE
            Texture* texture = ResourceManager::Instance()->loadTexture(str.C_Str(), str.C_Str(), GL_REPEAT, gammaCorrected);
            texture->setType(typeName);
            textures->push_back(texture);
        }

        return textures;
    }

    void Model::processAnimations(const aiScene* scene) {
        if (scene->HasAnimations()) {
            for (int i = 0; i < scene->mNumAnimations; i++) {
                animationMap[scene->mAnimations[i]->mName.C_Str()] = new Animation(this, scene->mAnimations[i], scene->mRootNode);
            }
            // For now start playing first animation automatically
            animator = new Animator(animationMap.begin()->second);
        }
    }

    void Model::setVertexBoneData(VertexP1N1T1B1UV& vertex, int boneID, float weight) {
        // Set bone weight for a vertex, up to 4 bones per vertex supported
        for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
            if (vertex.BoneIDs[i] < 0) {
                vertex.Weights[i] = weight;
                vertex.BoneIDs[i] = boneID;
                break;
            }
        }
    }

    void Model::extractBoneWeightForVertices(std::vector<VertexP1N1T1B1UV>& vertices, aiMesh* mesh, const aiScene* scene) {
        for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
            int boneID = -1;
            std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
            // Add new bone to info map if it's not added yet
            if (boneInfoMap.find(boneName) == boneInfoMap.end()) {
                BoneInfo boneInfo;
                boneInfo.id = boneCounter;
                boneInfo.offset = AssimpUtils::aiMatrixToGLM(mesh->mBones[boneIndex]->mOffsetMatrix);
                boneInfoMap[boneName] = boneInfo;
                boneID = boneCounter;
                boneCounter++;
            } else {
                // Else get existing bone id from info map
                boneID = boneInfoMap[boneName].id;
            }
            assert(boneID != -1);

            // Set this bone's weight for all vertices affected by it
            auto weights = mesh->mBones[boneIndex]->mWeights;
            int numWeights = mesh->mBones[boneIndex]->mNumWeights;
            for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex) {
                int vertexId = weights[weightIndex].mVertexId;
                float weight = weights[weightIndex].mWeight;
                assert(vertexId <= vertices.size());
                setVertexBoneData(vertices[vertexId], boneID, weight);
            }
        }
    }
}

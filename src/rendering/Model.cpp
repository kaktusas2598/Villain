#include "Model.hpp"

#include "Logger.hpp"
#include "ResourceManager.hpp"
#include <sstream>

namespace Villain {

    void Model::draw(Shader& shader) {
        for (auto& mesh: meshes) {
            // Draw mesh using it's own material
            mesh.draw(shader, materials[mesh.getMaterialName()]);
        }
    }

    void Model::loadModel(std::string path) {
        Assimp::Importer importer;
        // While loading scene, tell assimp to make sure uv coords are flipped along y axis
        // and all primitives are triangles
        // Other useful options:
        // aiProcess_GenNormals
        // aiProcess_CalcTangentSpace - won't calculate if there are no normals
        // NOTE: 2023-04-05: While doing tests with sponza model, disabled aiProcess_FlipUVs and model is working fine now
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::stringstream ss;
            ss << "ASSIMP ERROR: " << importer.GetErrorString();
            Logger::Instance()->error(ss.str().c_str());
            return;
        }
        directory = path.substr(0, path.find_last_of('/'));

        processNode(scene->mRootNode, scene);
    }

    void Model::processNode(aiNode* node, const aiScene* scene) {
        // process any meshes
        for (unsigned int i = 0; i < node->mNumMeshes; i ++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
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

            glm::vec3 tempVec;
            tempVec.x = mesh->mVertices[i].x;
            tempVec.y = mesh->mVertices[i].y;
            tempVec.z = mesh->mVertices[i].z;
            vertex.Position = tempVec;

            tempVec.x = mesh->mNormals[i].x;
            tempVec.y = mesh->mNormals[i].y;
            tempVec.z = mesh->mNormals[i].z;
            vertex.Normal = tempVec;

            tempVec.x = mesh->mTangents[i].x;
            tempVec.y = mesh->mTangents[i].y;
            tempVec.z = mesh->mTangents[i].z;
            vertex.Tangent = tempVec;

            tempVec.x = mesh->mBitangents[i].x;
            tempVec.y = mesh->mBitangents[i].y;
            tempVec.z = mesh->mBitangents[i].z;
            vertex.BiTangent = tempVec;


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

            // TODO: stop hardcoding specularity factor(32) here
            Material mat(matName, diffuseMap, 32.0f, specularMap, normalMap);
            mat.setDiffuseColor(diffuseColor);
            materials[matName] = mat;
            return Mesh<VertexP1N1T1B1UV>(vertices, indices, matName);
        }

        Logger::Instance()->warn("Mesh missing material information");
        return Mesh<VertexP1N1T1B1UV>(vertices, indices);
    }

    std::vector<Texture*>* Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName) {
        std::vector<Texture*>* textures = new std::vector<Texture*>();
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
            aiString str;
            mat->GetTexture(type, i, &str);
            // NOTE: 2023-04-16, Started setting here GL_REPEAT explicitely which is
            // default wrapping mode anyway and this fixes issues with 3D models, so it will probably stay
            // atm default mode in engine is GL_CLAMP_TO_EDGE
            Texture* texture = ResourceManager::Instance()->loadTexture(str.C_Str(), str.C_Str(), GL_REPEAT);
            texture->setType(typeName);
            textures->push_back(texture);
        }

        return textures;
    }
}

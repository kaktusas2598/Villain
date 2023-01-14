#include "Model.hpp"

#include <iostream>

void Model::draw(Shader& shader) {
    for (unsigned int i = 0; i < meshes.size(); i++) {
        meshes[i].draw(shader);
    }
}

void Model::loadModel(std::string path) {
    Assimp::Importer importer;
    // While loading scene, tell assimp to make sure uv coords are flipped along y axis
    // and all primitives are triangles
    // Other useful options:
    // aiProcess_GenNormals
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ASSIMP ERROR: " << importer.GetErrorString() << std::endl;
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

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture*> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;

        glm::vec3 tempVec;
        tempVec.x = mesh->mVertices[i].x;
        tempVec.y = mesh->mVertices[i].y;
        tempVec.z = mesh->mVertices[i].z;
        vertex.Position = tempVec;

        tempVec.x = mesh->mNormals[i].x;
        tempVec.y = mesh->mNormals[i].y;
        tempVec.z = mesh->mNormals[i].z;
        vertex.Normal = tempVec;

        // Check if texture coords are set, assimp supports up to 8 tex coords for each vertex
        if (mesh->mTextureCoords[0]) {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        } else {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
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
    }

    return Mesh(vertices, indices, textures, diffuseColor);
}

std::vector<Texture*>* Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName) {
    // TODO: In actual application you would check if texture was not already loaded by asset manager or sth like that
    std::vector<Texture*>* textures = new std::vector<Texture*>();
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        Texture* texture = new Texture(str.C_Str()); //directory
        texture->setType(typeName);
        textures->push_back(texture);
    }

    return textures;
}


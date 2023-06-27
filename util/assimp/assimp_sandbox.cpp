#include <cassert>
#include <map>
#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// Helper methods
static int spaceCount = 0;
void printSpace() {
    for (int i = 0; i < spaceCount; i++) {
        printf(" ");
    }
}

void printAssimpMatrix(const aiMatrix4x4& m) {
    printSpace(); printf("%f %f %f %f\n", m.a1, m.a2, m.a3, m.a4);
    printSpace(); printf("%f %f %f %f\n", m.b1, m.b2, m.b3, m.b4);
    printSpace(); printf("%f %f %f %f\n", m.c1, m.c2, m.c3, m.c4);
    printSpace(); printf("%f %f %f %f\n", m.d1, m.d2, m.d3, m.d4);
}


#define MAX_BONES_PER_VERTEX 4

struct VertexBoneData {
    uint BoneIDs[MAX_BONES_PER_VERTEX]= {0};
    float Weights[MAX_BONES_PER_VERTEX]= {0.0f};

    VertexBoneData() {}

    void addBoneData(uint boneId, float weight) {
        for (uint i = 0; i < MAX_BONES_PER_VERTEX; i++) {
            if (Weights[i] == 0.0) {
                BoneIDs[i] = boneId;
                Weights[i] = weight;
                return;
            }
        }

        // More bones affecting vertex than we support, should never get here
        //assert(0);
    }
};

// Nearly analogy to VertexP1N1BTUV class used in Model, but only for bone data
std::vector<VertexBoneData> vertexToBones;
// In Villain::Model class we store each mesh in separate VBO, but in this example we use this mesh base vertex vertex
// to pack all vertices in one bufffer so IDs for vertices and bones will be different here!
std::vector<int> meshBaseVertex;
std::map<std::string, uint> boneNameToIndexMap;

#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_JoinIdenticalVertices)

int getBoneId(const aiBone* bone) {
    int boneId = 0;
    std::string boneName(bone->mName.C_Str());

    if (boneNameToIndexMap.find(boneName) == boneNameToIndexMap.end()) {
        // Allocate index for new bone
        boneId = boneNameToIndexMap.size();
        boneNameToIndexMap[boneName] = boneId;
    } else {
        boneId = boneNameToIndexMap[boneName];
    }

    return boneId;
}

void parseSingleBone(int meshIndex, const aiBone* bone) {
    //printf("    Bone %d '%s' num vertices affected by this: %d\n", boneIndex, bone->mName.data, bone->mNumWeights);

    int boneId = getBoneId(bone);
    //printf("Bone Id: %d\n", boneId);

    //printAssimpMatrix(bone->mOffsetMatrix);

    for (int i = 0; i < bone->mNumWeights; i++) {
        //if (i == 0) printf("\n");
        const aiVertexWeight& weightInfo = bone->mWeights[i];

        uint globalVertexId = meshBaseVertex[meshIndex] + weightInfo.mVertexId;
        assert(globalVertexId < vertexToBones.size());
        vertexToBones[globalVertexId].addBoneData(boneId, weightInfo.mWeight);
        //printf("      %d: vertex id %d weight %f\n", i, bone->mWeights[i].mVertexId, bone->mWeights[i].mWeight);
    }

    //printf("\n");
}

void parseMeshBones(int meshIndex, const aiMesh* mesh) {
    for (int i = 0; i < mesh->mNumBones; i++) {
        parseSingleBone(meshIndex, mesh->mBones[i]);
    }
}

void parseMeshes(const aiScene* scene) {
    printf("------------------------\n");
    printf("Parsing %d meshes\n", scene->mNumMeshes);

    int totalVertices = 0;
    int totalIndices = 0;
    int totalBones = 0;

    meshBaseVertex.resize(scene->mNumMeshes);

    for (int i = 0; i < scene->mNumMeshes; i++) {
        const aiMesh* mesh = scene->mMeshes[i];
        int numVertices = mesh->mNumVertices;
        int numIndices = mesh->mNumFaces * 3;
        int numBones = mesh->mNumBones;
        meshBaseVertex[i] = totalVertices;
        printf("  Mesh %d '%s': vertices: %d indices: %d bones: %d\n", i, mesh->mName.data, numVertices, numIndices, numBones);
        totalVertices += numVertices;
        totalIndices += numIndices;
        totalBones += numBones;

        vertexToBones.resize(totalVertices);

        if (mesh->HasBones()) {
            parseMeshBones(i, mesh);
        }
        printf("\n");
    }

    printf("Total vertices: %d total indices: %d total bones: %d\n", totalVertices, totalIndices, totalBones);
}

void parseNode(const aiNode* node) {
    printSpace(); printf("Node name: '%s' children: %d meshes: %d\n", node->mName.C_Str(), node->mNumChildren, node->mNumMeshes);
    printSpace(); printf("Node transformations:\n");
    printAssimpMatrix(node->mTransformation);

    spaceCount += 4;

    for (int i = 0; i < node->mNumChildren; i++) {
        printf("\n");
        printSpace(); printf("--- %d --- \n", i);
        parseNode(node->mChildren[i]);
    }

    spaceCount -= 4;
}

void parseHierarchy(const aiScene* scene) {
    printf("------------------------\n");
    printf("Parsing node hierarchy\n");

    parseNode(scene->mRootNode);

}

void parseScene(const aiScene* scene) {
    parseMeshes(scene);

    parseHierarchy(scene);
}

/**
 *  This is utility program which takes path to 3D model file and prints information about it.
 *  it is used to debug animated models mainly
 *
 *  madvi11ain
 *  2023
 */
int main(int argc, char* argv[]) {

    if (argc != 2) {
        printf("Usage: %s <model filename>\n", argv[0]);
        return 1;
    }

    char* filename = argv[1];

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filename, ASSIMP_LOAD_FLAGS);

    printf("------------------------\n");
    printf("Loading model from %s\n", filename);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        printf("Error loading %s : %s\n", filename, importer.GetErrorString());
        return 1;
    }

    printf("Number of animations: %d, meshes: %d\n", scene->mNumAnimations, scene->mNumMeshes);

    parseScene(scene);

    return 0;
}



#include "MeshUtils.hpp"

namespace Villain {

    template <class VertexType>
    float MeshUtils<VertexType>::defaultUVMap[4] = {0.0f, 1.0f, 0.0f, 1.0f};

    template <class VertexType>
    void MeshUtils<VertexType>::addFace(std::vector<unsigned int>* indices, int startLocation, bool direction) {
        if (direction) {
            indices->push_back(startLocation + 2);
            indices->push_back(startLocation + 1);
            indices->push_back(startLocation + 0);
            indices->push_back(startLocation + 3);
            indices->push_back(startLocation + 2);
            indices->push_back(startLocation + 0);
        } else {
            indices->push_back(startLocation + 0);
            indices->push_back(startLocation + 1);
            indices->push_back(startLocation + 2);
            indices->push_back(startLocation + 2);//0
            indices->push_back(startLocation + 3);//2
            indices->push_back(startLocation + 0);//3
        }
    }

    template <class VertexType>
    void MeshUtils<VertexType>::addTangents(std::vector<VertexP1N1T1B1UV>* vertices, std::vector<unsigned int>* indices) {
        // Assuming we index by triangles
        for (int i = 0; i < indices->size(); i+=3) {
            VertexP1N1T1B1UV& i1 = (*vertices)[(*indices)[i]];
            VertexP1N1T1B1UV& i2 = (*vertices)[(*indices)[i + 1]];
            VertexP1N1T1B1UV& i3 = (*vertices)[(*indices)[i + 2]];
            glm::vec3 edge1 = i2.Position - i1.Position;
            glm::vec3 edge2 = i3.Position - i1.Position;
            glm::vec2 deltaUV1 = i2.UV - i1.UV;
            glm::vec2 deltaUV2 = i3.UV - i1.UV;
            float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

            i1.Tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
            i1.Tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
            i1.Tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
            i3.Tangent = i2.Tangent = i1.Tangent;

            i1.BiTangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
            i1.BiTangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
            i1.BiTangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
            i3.BiTangent = i2.BiTangent = i1.BiTangent;
        }
    }

    template <class VertexType>
    glm::vec3 MeshUtils<VertexType>::getNormal(const::glm::vec3& i1, const::glm::vec3& i2, const::glm::vec3& i3) {
        glm::vec3 v1 = i2 - i1;
        glm::vec3 v2 = i3 - i1;
        glm::vec3 v3 = glm::cross(v1, v2);
        return glm::normalize(v3);
    }

    // TODO: need ability to specify different orientations in easier way,
    // too much similar code in add XZ, XY and YZ plane methods
    template <class VertexType>
    void MeshUtils<VertexType>::addXZPlane(
            std::vector<VertexType>* vertices, std::vector<unsigned int>* indices,
            const glm::vec3& center, const glm::vec2& halfSize,
            float* uvCoords, bool direction) {
        addFace(indices, vertices->size(), direction);
        glm::vec3 normal;
        glm::vec2 minExtents = glm::vec2(center.x, center.z) - halfSize;
        glm::vec2 maxExtents = glm::vec2(center.x, center.z) + halfSize;

        glm::vec3 positions[4] = {
            {maxExtents.x, center.y, minExtents.y},
            {minExtents.x, center.y, minExtents.y},
            {minExtents.x, center.y, maxExtents.y},
            {maxExtents.x, center.y, maxExtents.y}};
        if (direction) // Bottom face
            normal = getNormal(positions[2], positions[1], positions[0]);
        else // Top face
            normal = getNormal(positions[0], positions[1], positions[2]);
        //printf("XZ plane normal: %fX, %fY, %fZ\n", normal.x, normal.y, normal.z);

        vertices->push_back({positions[0], normal, glm::vec2(uvCoords[1], uvCoords[3])});
        vertices->push_back({positions[1], normal, glm::vec2(uvCoords[0], uvCoords[3])});
        vertices->push_back({positions[2], normal, glm::vec2(uvCoords[0], uvCoords[2])});
        vertices->push_back({positions[3], normal, glm::vec2(uvCoords[1], uvCoords[2])});
    }

    template <class VertexType>
    void MeshUtils<VertexType>::addXYPlane(
            std::vector<VertexType>* vertices, std::vector<unsigned int>* indices,
            const glm::vec3& center, const glm::vec2& halfSize,
            float* uvCoords, bool direction) {
        addFace(indices, vertices->size(), direction);
        glm::vec3 normal;
        glm::vec2 minExtents = glm::vec2(center.x, center.y) - halfSize;
        glm::vec2 maxExtents = glm::vec2(center.x, center.y) + halfSize;

        glm::vec3 positions[4] = {
            {minExtents.x, minExtents.y, center.z},
            {minExtents.x, maxExtents.y, center.z},
            {maxExtents.x, maxExtents.y, center.z},
            {maxExtents.x, minExtents.y, center.z}};
        if (direction) // Back face
            normal = getNormal(positions[2], positions[1], positions[0]);
        else // Front face
            normal = getNormal(positions[0], positions[1], positions[2]);
        //printf("XY plane normal: %fX, %fY, %fZ\n", normal.x, normal.y, normal.z);

        vertices->push_back({positions[0], normal, glm::vec2(uvCoords[1], uvCoords[3])});
        vertices->push_back({positions[1], normal, glm::vec2(uvCoords[0], uvCoords[3])});
        vertices->push_back({positions[2], normal, glm::vec2(uvCoords[0], uvCoords[2])});
        vertices->push_back({positions[3], normal, glm::vec2(uvCoords[1], uvCoords[2])});
    }

    template <class VertexType>
    void MeshUtils<VertexType>::addYZPlane(
            std::vector<VertexType>* vertices, std::vector<unsigned int>* indices,
            const glm::vec3& center, const glm::vec2& halfSize,
            float* uvCoords, bool direction) {
        addFace(indices, vertices->size(), direction);
        glm::vec3 normal;
        glm::vec2 minExtents = glm::vec2(center.y, center.z) - halfSize;
        glm::vec2 maxExtents = glm::vec2(center.y, center.z) + halfSize;

        glm::vec3 positions[4] = {
            {center.x, maxExtents.x, maxExtents.y},
            {center.x, maxExtents.x, minExtents.y},
            {center.x, minExtents.x, minExtents.y},
            {center.x, minExtents.x, maxExtents.y}};
        if (direction) // Right face
            normal = getNormal(positions[2], positions[1], positions[0]);
        else // Left face
            normal = getNormal(positions[0], positions[1], positions[2]);
        //printf("YZ plane normal: %fX, %fY, %fZ\n", normal.x, normal.y, normal.z);

        vertices->push_back({positions[0], normal, glm::vec2(uvCoords[1], uvCoords[3])});
        vertices->push_back({positions[1], normal, glm::vec2(uvCoords[0], uvCoords[3])});
        vertices->push_back({positions[2], normal, glm::vec2(uvCoords[0], uvCoords[2])});
        vertices->push_back({positions[3], normal, glm::vec2(uvCoords[1], uvCoords[2])});

    }

    template <class VertexType>
    void MeshUtils<VertexType>::addAABB(std::vector<VertexType>* vertices, std::vector<unsigned int>* indices, const glm::vec3& center, const glm::vec3& halfSize) {
        glm::vec3 minExtents = center - halfSize;
        glm::vec3 maxExtents = center + halfSize;
        // Front Face
        glm::vec3 frontCenter = center; frontCenter.z += halfSize.z;
        addXYPlane(vertices, indices, frontCenter, glm::vec2(halfSize.x, halfSize.y), defaultUVMap, false);
        //Back Face
        glm::vec3 backCenter = center; backCenter.z -= halfSize.z;
        addXYPlane(vertices, indices, backCenter, glm::vec2(halfSize.x, halfSize.y), defaultUVMap, true);
        // Top Face
        glm::vec3 topCenter = center; topCenter.y += halfSize.y;
        addXZPlane(vertices, indices, topCenter, glm::vec2(halfSize.x, halfSize.z), defaultUVMap, false);
        // Bottom Face
        glm::vec3 bottomCenter = center; bottomCenter.y -= halfSize.y;
        addXZPlane(vertices, indices, bottomCenter, glm::vec2(halfSize.x, halfSize.z), defaultUVMap, true);
        // Left Face
        glm::vec3 leftCenter = center; leftCenter.x -= halfSize.y;
        addYZPlane(vertices, indices, leftCenter, glm::vec2(halfSize.y, halfSize.z), defaultUVMap, false);
        // Right Face
        glm::vec3 rightCenter = center; rightCenter.x += halfSize.y;
        addYZPlane(vertices, indices, rightCenter, glm::vec2(halfSize.y, halfSize.z), defaultUVMap, true);
    }

    // Credits go to: https://www.songho.ca/opengl/gl_sphere.html
    // NOTE: DebugRenderer has very very similar code to render debug sphere, should be reused
    template <class VertexType>
    void MeshUtils<VertexType>::addSphere(
            std::vector<VertexType>* vertices,
            std::vector<unsigned int>* indices,
            float radius, const glm::vec3& center,
            int sectors, int stacks) {
        float sectorCount = sectors; //<<< num of longitude divisions
        float stackCount = stacks; //<<< num of latitude divisions
        float x, y, z, xy; // Position
        float nx, ny, nz, lengthInv = 1.0f / radius; // Normal
        float s, t; // UV coords

        int start = vertices->size(); // Index for 1st new vertex added
        // NOTE: Both here and in DebugRenderer vertex and index count is HARDCODED!
        // It will change based on the number of sectors and stacks
        vertices->resize(vertices->size() + (stackCount + 1) * (sectorCount + 1));
        //REQUIRE(vertices.size() == (stacks + 1) * (sectors + 1));


        float sectorStep = 2 * M_PI / sectorCount;
        float stackStep = M_PI / stackCount;
        float sectorAngle, stackAngle;
        int vertexIndex = start;

        for (int i = 0; i <= stackCount; ++i) {
            stackAngle = M_PI / 2 - i * stackStep; //<<< from pi/2 to -pi/2
            xy = radius * cosf(stackAngle);
            z = radius * sinf(stackAngle);

            for (int j = 0; j <= sectorCount; ++j) {
                sectorAngle = j * sectorStep;
                (*vertices)[vertexIndex].Position.x = xy * cosf(sectorAngle);
                (*vertices)[vertexIndex].Position.y = xy * sinf(sectorAngle);
                (*vertices)[vertexIndex].Position.z = z;
                // shift sphere by center vector to transform it
                (*vertices)[vertexIndex].Position += center;

                nx = x * lengthInv;
                ny = y * lengthInv;
                nz = z * lengthInv;
                (*vertices)[vertexIndex].Normal = {nx, ny, nz};

                s = (float)j / sectorCount;
                t = (float)i / stackCount;
                (*vertices)[vertexIndex].UV = {s, t};

                vertexIndex++;
            }
        }
        indices->reserve(indices->size() + (stackCount - 1) * sectorCount * 6);
        //indices->reserve(indices->size() + 3672);
        unsigned int k1, k2;
        for (int i = 0; i < stackCount; ++i) {
            k1 = i * (sectorCount + 1); // beginning of current stack
            k2 = k1 + sectorCount + 1; // beginning of next stack
            for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
                // 2 triangles per sector excluding first and last stacks
                if (i != 0) {
                    (*indices).push_back(k1);
                    (*indices).push_back(k2);
                    (*indices).push_back(k1 + 1);
                }

                if (i != (stackCount - 1)) {
                    (*indices).push_back(k1 + 1);
                    (*indices).push_back(k2);
                    (*indices).push_back(k2 + 1);
                }
            }
        }
    }

    template void MeshUtils<VertexP1UV>::addFace(std::vector<unsigned int>*, int, bool);
    template void MeshUtils<VertexP1N1T1B1UV>::addTangents(std::vector<VertexP1N1T1B1UV>* vertices, std::vector<unsigned int>* indices);

    template void MeshUtils<VertexP1N1UV>::addXYPlane(std::vector<VertexP1N1UV>* vertices, std::vector<unsigned int>* indices, const glm::vec3& center, const glm::vec2& halfSize, float* uvCoords, bool direction);
    template void MeshUtils<VertexP1N1UV>::addXZPlane(std::vector<VertexP1N1UV>* vertices, std::vector<unsigned int>* indices, const glm::vec3& center, const glm::vec2& halfSize, float* uvCoords, bool direction);
    template void MeshUtils<VertexP1N1UV>::addYZPlane(std::vector<VertexP1N1UV>* vertices, std::vector<unsigned int>* indices, const glm::vec3& center, const glm::vec2& halfSize, float* uvCoords, bool direction);
    template void MeshUtils<VertexP1N1T1B1UV>::addXYPlane(std::vector<VertexP1N1T1B1UV>* vertices, std::vector<unsigned int>* indices, const glm::vec3& center, const glm::vec2& halfSize, float* uvCoords, bool direction);
    template void MeshUtils<VertexP1N1T1B1UV>::addXZPlane(std::vector<VertexP1N1T1B1UV>* vertices, std::vector<unsigned int>* indices, const glm::vec3& center, const glm::vec2& halfSize, float* uvCoords, bool direction);
    template void MeshUtils<VertexP1N1T1B1UV>::addYZPlane(std::vector<VertexP1N1T1B1UV>* vertices, std::vector<unsigned int>* indices, const glm::vec3& center, const glm::vec2& halfSize, float* uvCoords, bool direction);

    template void MeshUtils<VertexP1N1UV>::addAABB(std::vector<VertexP1N1UV>* vertices, std::vector<unsigned int>* indices, const glm::vec3& center, const glm::vec3& halfSize);
    template void MeshUtils<VertexP1N1T1B1UV>::addAABB(std::vector<VertexP1N1T1B1UV>* vertices, std::vector<unsigned int>* indices, const glm::vec3& center, const glm::vec3& halfSize);

    template void MeshUtils<VertexP1N1UV>::addSphere(std::vector<VertexP1N1UV>*, std::vector<unsigned int>*, float , const glm::vec3&, int, int);
    template void MeshUtils<VertexP1N1T1B1UV>::addSphere(std::vector<VertexP1N1T1B1UV>*, std::vector<unsigned int>*, float, const glm::vec3&, int, int);
}

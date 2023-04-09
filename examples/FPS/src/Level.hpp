#pragma once

#include "Bitmap.hpp"
#include "Door.hpp"
#include "Mesh.hpp"

class Level {
    public:
        Level(const std::string& fileName, const std::string& tileAtlasFileName);

        void update(float deltaTime);

        inline Villain::Mesh<VertexP1N1UV>* getMesh() const { return mesh; }
        inline const Villain::Material& getMaterial() const { return *material; }

        // TODO: need to incorporate level collision into Physics Engine
        glm::vec3 checkCollisions(const glm::vec3& oldPos, const glm::vec3& newPos, float objectWidth, float objectLength);
        void setDoor(Door* d) { door = d; }
    private:
        void generateLevel(const std::string& tileAtlasFileName);

        // Generate indices for quad
        void addFace(std::vector<unsigned int>* indices, int startLocation, bool direction);
        // Generate vertices for quad on any plane, used for walls/floors and ceilings
        void addVertices(std::vector<VertexP1N1UV>* vertices, int i, int j, bool x, bool y, bool z, float offset, float* texCoords);

        glm::vec2 rectCollide(glm::vec2& oldPos, glm::vec2& newPos, glm::vec2& objectSize, glm::vec2& blockSize, glm::vec2& blockPos);

        // Selects tile from atlas using some value, most likely color component of bitmap
        float* getTexCoords(int val);

        Bitmap* bitmap = nullptr;
        Villain::Mesh<VertexP1N1UV>* mesh = nullptr;
        Villain::Material* material = nullptr;

        // TEMP
        Door* door = nullptr;
};

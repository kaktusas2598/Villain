#pragma once

#include "Application.hpp"
#include "Bitmap.hpp"
#include "Door.hpp"
#include "Mesh.hpp"

class Level {
    public:
        Level(const std::string& fileName, const std::string& tileAtlasFileName, Villain::Application* app);

        void update(float deltaTime);

        inline Villain::Mesh<VertexP1N1UV>* getMesh() const { return mesh; }
        inline const Villain::Material& getMaterial() const { return *material; }

        // TODO: need to incorporate level collision into Physics Engine
        glm::vec3 checkCollisions(const glm::vec3& oldPos, const glm::vec3& newPos, float objectWidth, float objectLength);
        std::vector<Door*>& getDoors() { return doors; }
        void openDoors(const glm::vec3& pos);
        inline Villain::SceneNode* getNode() { return levelNode; }

        static const float OPEN_DOOR_DISTANCE;
    private:
        void generateLevel(const std::string& tileAtlasFileName);
        void addSpecialObject(int blueValue, int x, int y);
        void addDoor(int x, int y);

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

        Villain::Application* application;
        Villain::SceneNode* levelNode;
        // Could just use children nodes instead but that needs dynamic casts,
        // so we need to introduce maybe component signature and systems and...
        std::vector<Door*> doors;
};

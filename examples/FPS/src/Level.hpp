#pragma once

#include "Application.hpp"
#include "Bitmap.hpp"
#include "Door.hpp"
#include "Mesh.hpp"

class Medkit;
class Monster;
class Player;

class Level {
    public:
        Level(const std::string& fileName, const std::string& tileAtlasFileName, Villain::Application* app);
        ~Level() { delete bitmap; }

        void update(float deltaTime);

        inline Villain::Mesh<VertexP1N1UV>* getMesh() const { return mesh; }
        inline const Villain::Material& getMaterial() const { return *material; }

        // TODO: need to incorporate level collision into Physics Engine
        glm::vec3 checkCollisions(const glm::vec3& oldPos, const glm::vec3& newPos, float objectWidth, float objectLength);
        glm::vec2 checkIntersections(const glm::vec2& lineStart, const glm::vec2& lineEnd, bool hurtMonsters = false);
        glm::vec2 lineIntersectRect(const glm::vec2& lineStart, const glm::vec2& lineEnd, glm::vec2& rectPos, glm::vec2& rectSize);

        std::vector<Door*>& getDoors() { return doors; }
        Player* getPlayer() const { return player; }
        void damagePlayer(int amount);
        void openDoors(const glm::vec3& pos, bool exitLevel = false);
        void removeMedkit(Villain::SceneNode* medkit);
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

        glm::vec2 lineIntersect(const glm::vec2& lineStart1, const glm::vec2& lineEnd1, const glm::vec2& lineStart2, const glm::vec2& lineEnd2);
        glm::vec2 rectCollide(glm::vec2& oldPos, glm::vec2& newPos, glm::vec2& objectSize, glm::vec2& blockSize, glm::vec2& blockPos);
        glm::vec2 findNearestVec2(const glm::vec2& a, const glm::vec2& b, const glm::vec2& relativePos);
        float vec2Cross(const glm::vec2& a, const glm::vec2& b) { return a.x * b.y - a.y * b.x; }

        // Selects tile from atlas using some value, most likely color component of bitmap
        float* getTexCoords(int val);

        Bitmap* bitmap = nullptr;
        Villain::Mesh<VertexP1N1UV>* mesh = nullptr;
        Villain::Material* material = nullptr;

        // Collision lines of level, really should be handled by physics engine
        std::vector<glm::vec2> collisionPosStart;
        std::vector<glm::vec2> collisionPosEnd;

        Villain::Application* application;
        Villain::SceneNode* levelNode;
        // Could just use children nodes instead but that needs dynamic casts,
        // so we need to introduce maybe component signature and systems and...
        std::vector<Door*> doors;

        std::vector<Monster*> enemies;
        std::vector<Villain::SceneNode*> medkits;
        Player* player = nullptr;
        std::vector<glm::vec3> exitPoints;
};

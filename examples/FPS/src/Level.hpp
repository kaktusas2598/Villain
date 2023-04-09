#pragma once

#include "Bitmap.hpp"
#include "Mesh.hpp"

class Level {
    public:
        Level(const std::string& fileName, const std::string& tileAtlasFileName);

        void update(float deltaTime);

        inline Villain::Mesh<VertexP1N1UV>* getMesh() const { return mesh; }
        inline const Villain::Material& getMaterial() const { return *material; }
    private:
        void generateLevel(const std::string& tileAtlasFileName);

        // Generate indices for quad
        void addFace(std::vector<unsigned int>* indices, int startLocation, bool direction);
        // Generate vertices for quad on any plane, used for walls/floors and ceilings
        void addVertices(std::vector<VertexP1N1UV>* vertices, int i, int j, bool x, bool y, bool z, float offset, float* texCoords);

        // Selects tile from atlas using some value, most likely color component of bitmap
        float* getTexCoords(int val);

        Bitmap* bitmap = nullptr;
        Villain::Mesh<VertexP1N1UV>* mesh = nullptr;
        Villain::Material* material = nullptr;
};

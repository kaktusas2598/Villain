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
        void addFace(std::vector<unsigned int>* indices, int startLocation, bool direction);
        float* getTexCoords(int val);

        Bitmap* bitmap = nullptr;
        Villain::Mesh<VertexP1N1UV>* mesh = nullptr;
        Villain::Material* material = nullptr;
};

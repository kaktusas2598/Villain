#pragma once

#include "rendering/Texture.hpp"

namespace Villain {

    const int MAX_TEXTURE_TILES = 4;

    // Used to blend terrain textures based on terrain height value
    struct TextureHeightDesc {
        float Low = 0.0f;
        float Optimal = 0.0f;
        float High = 0.0f;

        void print() const { printf("Low %f Optimal %f High %f\n", Low, Optimal, High); }
    };

    struct TextureTile {
        Villain::Texture* Tex;
        TextureHeightDesc HeightDesc;
    };

    class Terrain;

    class TextureGenerator {
        public:
            TextureGenerator() {}

            void loadTile(const char* fileName);
            Villain::Texture* generateTexture(int textureSize, Terrain* terrain, float minHeight, float maxHeight);

        private:
            void calculateTextureRegions(float minHeight, float maxHeight);
            // Calculate fraction for texture's weight in specified height
            float regionPercent(int tile, float height);

            TextureTile textureTiles[MAX_TEXTURE_TILES] = {};
            int numTextureTiles = 0;
    };
}

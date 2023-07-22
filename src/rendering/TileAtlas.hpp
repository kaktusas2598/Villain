#pragma once

#include "Texture.hpp"
#include "glm/glm.hpp"
#include <iostream>

namespace Villain {

    class TileAtlas {
        public:
            void init(Texture* t, const glm::ivec2& tileDims) {
                texture = t;
                dims = tileDims;
            }

            // Get Texture Coordinates for a tile in atlas,
            // 0 is the bottom left frame/tile
            glm::vec4 getUVs(int index) {
                int xTile = index % dims.x;
                int yTile = index / dims.x;

                glm::vec4 uv;
                uv.x = xTile / (float)dims.x;
                uv.y = yTile / (float)dims.y;
                uv.z = 1.0f / dims.x;
                uv.w = 1.0f / dims.y;

                return uv;
            }

            // Get Texture Coordinates for a tile in atlas, where
            // 0th row is the top one
            glm::vec4 getUVs(int row, int column) {
                glm::vec4 uv;
                uv.x = column / (float)dims.x;
                uv.y = (dims.y - row - 1) / (float)dims.y;
                uv.z = 1.0f / dims.x;
                uv.w = 1.0f / dims.y;

                return uv;
            }

            Texture* texture;
            // Rows/columns in sprite/tile atlas/sheet
            glm::ivec2 dims;
    };
}

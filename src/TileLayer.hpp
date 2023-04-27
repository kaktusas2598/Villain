#ifndef __TileLayer__
#define __TileLayer__

#include <vector>
#include "Layer.hpp"
#include "glm/glm.hpp"
#include "rendering/SpriteBatch.hpp"

namespace Villain {

    struct TileSet;

    class TileLayer : public Layer {
        public:
            TileLayer(int tilesize, double scale, const std::vector<TileSet> &tilesets);

            // Calculates number of tile rows and columns shown on screen, can be called on window resize event too
            void init();
            virtual void update();
            virtual void render(SpriteBatch *batch, Camera* cam);

            void setTileIDs(const std::vector<std::vector<int>> &data) { tileIDs = data; }
            void setTileSize(int tilesize) { tileSize = tilesize; }
            void setMapWidth(int w) { mapWidth = w; }
            void setMapHeight(int h) { mapHeight = h; }

            int getTileSize() const { return tileSize; }
            double getScale() const { return scale; }
            int getMapWidth() const { return mapWidth; }
            int getMapHeight() const { return mapHeight; }
            const std::vector<std::vector<int>>& getTileIDs() { return tileIDs; }
            glm::vec2 getPosition() const { return position; }

			void setVisible(bool visible) { isVisible = visible; }
			bool getVisible() { return isVisible; }

			void setDepth(float z) { depth = z; }
			float getDepth() const { return depth; }

        private:
            int tileSize;
            double scale;
            float depth;

            const std::vector<TileSet> &tileSets;

            // For scrolling maps
            glm::vec2 position;
            glm::vec2 velocity;

            // Useful for collision layer
            bool isVisible;

            // Tilesheet properties
            int numColumns;
            int numRows;

            int mapHeight;
            int mapWidth;

            std::vector<std::vector<int>> tileIDs;

            TileSet getTilesetByID(int tileID);
    };
}

#endif // __TileLayer__

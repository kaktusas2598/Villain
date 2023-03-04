#include "TileLayer.hpp"

#include "Engine.hpp"
#include "Level.hpp"

#include "Logger.hpp"
#include "ResourceManager.hpp"

namespace Villain {
    TileLayer::TileLayer (int tilesize, double scale, const std::vector<TileSet> &tilesets) : tileSize(tilesize), scale(scale), tileSets(tilesets), position(0,0), velocity(0,0), isVisible(true) {
        init();
    }

    void TileLayer::init() {
        // Number of tile columns and rows needed to fill the screen
        // Adding 2 to each fixes gaps in rendering at the far sides of the screen, but TileLayer and collision system needs to check array bounds
        numColumns = TheEngine::Instance()->getScreenWidth() / (tileSize * scale) + 2;
        numRows = TheEngine::Instance()->getScreenHeight() / (tileSize * scale) + 2;

        std::cout << "Num columns: " << numColumns << "\n";
        std::cout << "Num rows: " << numRows << "\n";
        std::cout << "Tilesize: " << tileSize << "\n";
    }

    void TileLayer::update() {
        // For scrolling maps
        // if(position.getX() < ((mapWidth * tileSize) - TheEngine::Instance()->getGameWidth()) - tileSize) {
        //     velocity.setX(TheEngine::Instance()->getScrollSpeed());
        //     position += velocity;
        // } else {
        //     velocity.setX(0);
        // }
    }

    void TileLayer::render(SpriteBatch* batch, Camera2D* cam) {
		if (!isVisible) {
			return;
		}

        int x, y, x2, y2 = 0;

	    // Tile map position based on camera - causes weird rendering issues
        // TODO: not sure if Engine class should store camera anymore, maybe level should store active camera used?
        //position.x = cam->getPosition().x;
        //position.y = cam->getPosition().y;
        position.x = 0;
        position.y = 0;
        //position.x = TheEngine::Instance()->camera.x;
        //position.y = TheEngine::Instance()->camera.y;

        x = position.x / (tileSize * scale);
        y = position.y / (tileSize * scale);

        x2 = int(position.x) % int(tileSize * scale);
        y2 = int(position.y) % int(tileSize * scale);

        for (int i = 0; i < numRows; i++) {
            for (int j = 0; j < numColumns; j++) {

                // Do not try to render tiles out of map boundaries
                if (i + y >= mapHeight || j + x >= mapWidth)
                    continue;

                int id = tileIDs[i + y][j + x]; //i + y
                if (id == 0) {
                    continue;
                }

                //if(((j * tileSize) - x2) - TheEngine::Instance()->camera.x < -tileSize ||
                    //((j * tileSize) - x2) - TheEngine::Instance()->camera.x > TheEngine::Instance()->getScreenWidth()) {
                    //continue;
                //}

                TileSet tileset = getTilesetByID(id);
                id--;

                // draw the tile into position while offsetting its x position by subtracting the camera position
                Texture* tilesetTexture = ResourceManager::Instance()->getTexture(tileset.name);
                int tileX = ((j * tileSize * scale) - x2);
                int tileY = ((i * tileSize * scale) - y2);
                glm::vec4 destRect(tileX, tileY, tileSize * scale, tileSize * scale);
                int row = (id - (tileset.firstGridID - 1)) / tileset.numColumns;
                int column = (id - (tileset.firstGridID - 1)) % tileset.numColumns;
                //std::cout << "ID: " << id << ", i: " << i << ", j: " << j << ", row: " << row << ", column: " << column << "\n";
                // TODO:Utilise depth and color, spacing and margin can be implemented if needed
                batch->draw(destRect, column, row, tileset.tileWidth, tileset.tileHeight, tilesetTexture, 0.0f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
            }
        }
    }

    TileSet TileLayer::getTilesetByID(int tileID) {
        for (size_t i = 0; i < tileSets.size(); i++) {
           if( i + 1 <= tileSets.size() - 1) {
                if(tileID >= tileSets[i].firstGridID && tileID < tileSets[i +1].firstGridID) {
                    return tileSets[i];
                }
            } else {
                return tileSets[i];
            }
        }
        Logger::Instance()->error("Could not find tileset, returning empty one.");
        TileSet t;
        return t;
    }
}

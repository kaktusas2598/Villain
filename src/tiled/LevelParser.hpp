#pragma once;

#include "tinyxml2/tinyxml2.h"
#include <vector>
#include <string>

namespace Villain {

    class Level;
    class Layer;
    class CollisionLayer;
    struct TileSet;
    class TileLayer;

    class LevelParser {
        public:
            Level* parseLevel(const char* levelFile);
        private:
            void parseTileSets(tinyxml2::XMLElement *tilesetRoot, Level* level);
            void parseTextures(tinyxml2::XMLElement *textureRoot, Level* level);
            void parseTileLayer(
                tinyxml2::XMLElement *tileElement,
                std::vector<Layer*>* layers,
                const std::vector<TileSet>* tilesets,
                std::vector<TileLayer*>* collisionLayers
            );
            void parseObjectLayer(tinyxml2::XMLElement *objectRoot, std::vector<Layer*>* layers);
            CollisionLayer* parseCollisionLayer(tinyxml2::XMLElement* collisionRoot, std::vector<Layer*>* layers);

            int tileSize;
            int width;
            int height;
	    double scale;
    };

}

#include "LevelParser.hpp"
#include "Level.hpp"
#include "Logger.hpp"
#include "TileLayer.hpp"
//#include "ObjectLayer.hpp"
#include "CollisionLayer.hpp"
#include "ResourceManager.hpp"
//#include "Entity.hpp"
#include <algorithm>
#include <string>
#include <sstream>

// NOTE: For SDL_Rect, want to replace with own structure for representing colliders
// Or can easily use something like glm::vec4
#include <SDL2/SDL.h>

namespace Villain {

    Level* LevelParser::parseLevel(const char* levelFile) {
        std::string logMessage = "Loading tile map: " + std::string(levelFile);
        Logger::Instance()->info(logMessage.data());

        tinyxml2::XMLDocument levelDoc;
        levelDoc.LoadFile(levelFile);

        Level* level = new Level();

        tinyxml2::XMLElement* root = levelDoc.RootElement();
        // Don't see a reason to store these params in LevelParser, they are unique to each level
        tileSize = root->IntAttribute("tilewidth");
        width = root->IntAttribute("width");
        height = root->IntAttribute("height");

        scale = 1.0f;

        // Parse properties for entity texture sources
        for (tinyxml2::XMLElement* e = root->FirstChildElement(); e != NULL; e = e->NextSiblingElement()) {
            if (e->Value() == std::string("properties")) {
                parseTextures(e, level);
            }
        }

        level->setWidth(tileSize * width * scale);
        level->setHeight(tileSize * height * scale);

        for (tinyxml2::XMLElement* e = root->FirstChildElement(); e != NULL; e = e->NextSiblingElement()) {
            if (e->Value() == std::string("tileset")) {
                parseTileSets(e, level);
            }
        }

        // Parse object and tile layers
        for (tinyxml2::XMLElement* e = root->FirstChildElement(); e != NULL; e = e->NextSiblingElement()) {
            if (e->Value() == std::string("objectgroup") || e->Value() == std::string("layer")) {
                if (e->FirstChildElement()->Value() == std::string("object")) {
                    // This will crash if creating default object layer in Tiled because it doesn't attach type attribute
                    // and actually seems to delete it if you save map again
                    /*if (e->Attribute("type") == std::string("entity")) {
                        parseObjectLayer(e, level->getLayers());
                    } else if (e->Attribute("type") == std::string("collider")) {
                        CollisionLayer* colLayer = parseCollisionLayer(e, level->getLayers());
                        level->setCollisionLayer(colLayer);
                    }*/
                    // Tile Layer or tile layer with properties such as "collidable"
                } else if (e->FirstChildElement()->Value() == std::string("data") ||
                    (e->FirstChildElement()->NextSiblingElement() != 0
                        && e->FirstChildElement()->NextSiblingElement()->Value() == std::string("data"))
                        ) {
                    Logger::Instance()->info("Loading tile layer.");
                    std::ostringstream ss;
                    ss << "Map width: " << width << ", Map height: " << height;
                    Logger::Instance()->info(ss.str().c_str());
                    parseTileLayer(e, level->getLayers(), level->getTilesets(), level->getCollisionLayers());
                }
            }
        }

        return level;
    }

    void LevelParser::parseTileSets(tinyxml2::XMLElement *tilesetRoot, Level* level) {
        // Load tileset textures
        ResourceManager::Instance()->loadTexture(
            tilesetRoot->FirstChildElement()->Attribute("source"),
            tilesetRoot->Attribute("name")
        );
        level->addTexture(tilesetRoot->Attribute("name"));

        TileSet tileset;
        tileset.spacing = 0;
        tileset.margin = 0;

        tileset.width = tilesetRoot->FirstChildElement()->IntAttribute("width");
        tileset.height = tilesetRoot->FirstChildElement()->IntAttribute("height");
        tileset.firstGridID = tilesetRoot->IntAttribute("firstgid");
        tileset.tileWidth = tilesetRoot->IntAttribute("tilewidth");
        tileset.tileHeight = tilesetRoot->IntAttribute("tileheight");
        if (tilesetRoot->Attribute("spacing"))
			tileset.spacing = tilesetRoot->IntAttribute("spacing");
        if (tilesetRoot->Attribute("margin"))
			tileset.margin = tilesetRoot->IntAttribute("margin");
        tileset.name = tilesetRoot->Attribute("name");

        //tileset.numColumns = tileset.width / (tileset.tileWidth + tileset.spacing);
        tileset.numColumns = tilesetRoot->IntAttribute("columns");

        level->getTilesets()->push_back(tileset);

    }

    void LevelParser::parseTileLayer(tinyxml2::XMLElement *tileElement, std::vector<Layer*>* layers, const std::vector<TileSet>* tilesets, std::vector<TileLayer*>* collisionLayers) {
        TileLayer* layer = new TileLayer(tileSize, scale, *tilesets);
        bool collidable = false;

        std::vector<std::vector<int>> data; // tile data
        // std::string decodedIDs;
        tinyxml2::XMLElement *dataNode = nullptr;
        for(tinyxml2::XMLElement* e = tileElement->FirstChildElement(); e != NULL; e = e->NextSiblingElement()) {
            if(e->Value() == std::string("properties")) {
                for(tinyxml2::XMLElement* property = e->FirstChildElement(); property != NULL; property = property->NextSiblingElement()) {
                    if(property->Value() == std::string("property")) {
                        if (property->Attribute("name") == std::string("collidable")) {
                            collidable = true;
                        } else if (property->Attribute("name") == std::string("depth")) {
                            layer->setDepth(property->FloatAttribute("value"));
                        }
                    }
                }
            }

            if(e->Value() == std::string("data")) {
                dataNode = e;
            }
        }

        if (!dataNode) {
            exitWithError("Tile map data node not found");
        }

        std::string t; // mine
        for (tinyxml2::XMLNode* e = dataNode->FirstChild(); e != NULL; e = e->NextSibling()) {
            tinyxml2::XMLText* text = e->ToText();
            /*std::string*/ t = text->Value();
            // decodedIDs = base64_decode(t);
        }

        // Parsing CSV map data, probably not the best solution, need to investigate how much memory this uses
        std::vector<int> layerRow;
        std::string row;
        int r = 0;
        std::istringstream tokenStream(t);
        while(std::getline(tokenStream, row)) {//NOTE: 3rd param used to be ' ' in vigilant for some reason
            if (row.empty()) //NOTE: why first row is empty?
                continue;
            layerRow.clear();
            std::istringstream rowStream(row);
            std::string tileId;
            while(std::getline(rowStream, tileId, ',')) {
                layerRow.push_back(stoi(tileId));
            }
            data.push_back(layerRow);
        }

        // uncompress zlib compression
        // uLongf numGids = width * height * sizeof(int);
        // long numGids = width * height * sizeof(int);
        // std::vector<unsigned> gids(numGids);
        // uncompress((Bytef*)&gids[0], &numGids,(const
        // Bytef*)decodedIDs.c_str(), decodedIDs.size());

        // std::vector<int> layerRow(width);
        // for (int j = 0; j < height; j++) {
        //     data.push_back(layerRow);
        // }

        // for (int rows = 0; rows < height; rows++) {
        //     for (int cols = 0; cols < width; cols++) {
        //         // data[rows][cols] = gids[rows * width + cols];
        //     }
        // }

        // 2023-03-17: Reversing tileIDs array will make sure row 0 is at the bottom instead of top
        // because we are using OpenGL now
        std::reverse(data.begin(), data.end());

        layer->setTileIDs(data);
        layer->setMapWidth(width);
        layer->setMapHeight(height);

        //Initialise game camera
        // TheEngine::Instance()->camera = {0, 0, TheEngine::Instance()->getScreenWidth(), TheEngine::Instance()->getScreenHeight()};

        // NOTE: In Vigilant, collidable attribute was used for invisible collision layers, not sure if this is still neccessary
        // as we can show collidable tiles using shader, will have to decide the function for this
        if (collidable) {
            //layer->setVisible(false);
            collisionLayers->push_back(layer);
        }

        layers->push_back(layer);
    }

    void LevelParser::parseTextures(tinyxml2::XMLElement *textureRoot, Level* level) {
        for (tinyxml2::XMLElement* e = textureRoot->FirstChildElement(); e != NULL; e = e->NextSiblingElement()) {
            if (e->Attribute("name") == std::string("scale")) {
                scale = e->DoubleAttribute("value");
            } else {
                ResourceManager::Instance()->loadTexture(e->Attribute("value"), e->Attribute("name"));
                level->addTexture(e->Attribute("name"));
            }
        }
    }

    // NOT USED at the moment, because Entity Factory has been removed, need to refactor this to use Lua Entitites instead
    void LevelParser::parseObjectLayer(tinyxml2::XMLElement *objectRoot, std::vector<Layer*>* layers) {
        // TODO: ???
        // Do we want to keep object layer at all
        /*ObjectLayer* objLayer = new ObjectLayer();

        for (TiXmlElement* e = objectRoot->FirstChildElement(); e != NULL; e = e->NextSiblingElement()) {
            if (e->Value() == std::string("object")) {
                int x, y, width, height, numFrames, callbackId = 0;
                std::string textureId;

                e->Attribute("x", &x);
                e->Attribute("y", &y);
                e->Attribute("width", &width);
                e->Attribute("height", &height);
                //Entity* entity = TheEntityFactory::Instance()->create(e->Attribute("class"));

                for (TiXmlElement* properties = e->FirstChildElement(); properties != NULL; properties = properties->NextSiblingElement()) {
                    if (properties->Value() == std::string("properties")) {
                        for (TiXmlElement* property = properties->FirstChildElement(); property != NULL; property = property->NextSiblingElement()) {
                            if (property->Attribute("name") == std::string("numFrames")) {
                                property->Attribute("value", &numFrames);
                            } else if (property->Attribute("name") == std::string("textureID")) {
                                textureId = property->Attribute("value");
                            } else if (property->Attribute("name") == std::string("callbackID")) {
                                property->Attribute("value", &callbackId);
                            }
                        }
                    }
                }
                //entity->load(new LoaderParams(x, y, width, height, textureId, numFrames, callbackId));
                //objLayer->getEntities()->push_back(entity);
            }
        }
        layers->push_back(objLayer);*/
    }

    CollisionLayer* LevelParser::parseCollisionLayer(tinyxml2::XMLElement* collisionRoot, std::vector<Layer*>* layers) {
        CollisionLayer *colLayer = new CollisionLayer();

        for (tinyxml2::XMLElement* e = collisionRoot->FirstChildElement(); e != NULL; e = e->NextSiblingElement()) {
            int x, y, width, height;

            x = e->IntAttribute("x");
            y = e->IntAttribute("y");
            width = e->IntAttribute("width");
            height = e->IntAttribute("height");
            // Entity* entity = TheEntityFactory::Instance()->create(e->Attribute("class"));
            // entity->load(new LoaderParams(x, y, width, height, textureId, numFrames, callbackId));
            colLayer->getColliders().push_back(SDL_Rect{x, y, width, height});

        }
        return colLayer;
    }
}

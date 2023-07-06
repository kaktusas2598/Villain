#include "SceneParser.hpp"

#include "Engine.hpp"
#include "camera/Camera.hpp"
#include "Logger.hpp"
#include "SceneNode.hpp"
#include "components/ModelRenderer.hpp"
#include "components/MoveController.hpp"
#include <sstream>

namespace Villain {

    std::string GetUniqueId() {
       static int n = 1;
       std::ostringstream os;
       os << n++;
       return os.str();
    }

    void SceneParser::loadSceneGraph(const std::string& fileName, SceneNode* rootNode) {
        tinyxml2::XMLDocument xmlDoc;

        Logger::Instance()->warn("Loading scene graph: {}", fileName);
        if (xmlDoc.LoadFile(fileName.c_str()) != tinyxml2::XML_SUCCESS) {
            Logger::Instance()->error("Could not open scene graph XML file {} Error: {}", fileName, xmlDoc.ErrorName());
            return;
        }

        tinyxml2::XMLElement *root = xmlDoc.RootElement();

        for (tinyxml2::XMLElement *e = root->FirstChildElement(); e != NULL; e = e->NextSiblingElement()) {
            if (e->Value() == std::string("Node"))
                    parseSceneNode(e, rootNode);

            // TODO:
            //if (e->Value() == std::string("Properties"))
                    //parseProperties(e, rootNode);
        }
    }

    void SceneParser::saveSceneGraph(const std::string& fileName, SceneNode* rootNode) {
        // TODO:
    }

    void SceneParser::parseSceneNode(tinyxml2::XMLElement* nodeRoot, SceneNode* parentNode) {
        std::string nodeName = GetUniqueId();
        const char* nameAttr = nodeRoot->Attribute("name");
        if (nameAttr)
            nodeName = nameAttr;


        // Optional Attribs for node
        float scale = 1.0f, positionX = 0.0f, positionY = 0.0f, positionZ = 0.0f;
        nodeRoot->QueryFloatAttribute("scale", &scale);
        nodeRoot->QueryFloatAttribute("positionX", &positionX);
        nodeRoot->QueryFloatAttribute("positionY", &positionY);
        nodeRoot->QueryFloatAttribute("positionZ", &positionZ);

        SceneNode* currentNode = new SceneNode(nodeName, {positionX, positionY, positionZ}, {}, scale);

        for (tinyxml2::XMLElement *e = nodeRoot->FirstChildElement(); e != NULL; e = e->NextSiblingElement()) {
            // Parse components
            if (e->Value() == std::string("Components")) {
                for (tinyxml2::XMLElement *component = e->FirstChildElement(); component != NULL; component = component->NextSiblingElement()) {

                    if (component->Attribute("type") == std::string("Model")) {
                        std::string modelPath = component->Attribute("fileName");
                        currentNode->addComponent(new ModelRenderer(new Model(modelPath.c_str())));
                    }

                    if (component->Attribute("type") == std::string("Camera")) {
                        Camera* camera = new Camera();
                        // Optional camera values
                        float zNear, zFar;
                        bool isMain = false;

                        if (component->QueryFloatAttribute("zNear", &zNear) == tinyxml2::XML_SUCCESS) {
                            camera->setZNear(zNear);
                        }
                        if (component->QueryFloatAttribute("zFar", &zFar) == tinyxml2::XML_SUCCESS) {
                            camera->setZFar(zFar);
                        }

                        //if (component->QueryBoolAttribute("main", &isMain) == tinyxml2::XML_SUCCESS) {
                            //parentNode->getEngine()->getRenderingEngine()->setMainCamera(*camera);
                        //}

                        camera->rescale(Engine::getScreenWidth(), Engine::getScreenHeight());
                        currentNode->addComponent(camera);
                    }

                    if (component->Attribute("type") == std::string("MoveController")) {
                        currentNode->addComponent(new MoveController());
                    }

                    // TODO: Rest of components
                }
            }
            // Parse children
            if (e->Value() == std::string("Children")) {
                for (tinyxml2::XMLElement *child = e->FirstChildElement(); child != NULL; child = child->NextSiblingElement()) {
                    parseSceneNode(child, currentNode);
                }
            }
        }
        parentNode->addChild(currentNode);
    }
}

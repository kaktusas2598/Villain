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

        VILLAIN_INFO("Loading scene graph: {}", fileName);
        if (xmlDoc.LoadFile(fileName.c_str()) != tinyxml2::XML_SUCCESS) {
            VILLAIN_ERROR("Could not open scene graph XML file {} Error: {}", fileName, xmlDoc.ErrorName());
            return;
        }

        tinyxml2::XMLElement *root = xmlDoc.RootElement();

        for (tinyxml2::XMLElement *e = root->FirstChildElement(); e != NULL; e = e->NextSiblingElement()) {
            if (e->Value() == std::string("Node"))
                    parseSceneNode(e, rootNode);

            if (e->Value() == std::string("Properties"))
                    parseProperties(e, rootNode);
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
        float scale = 1.0f, positionX = 0.0f, positionY = 0.0f, positionZ = 0.0f, rotX = 0.0f, rotY = 0.0f, rotZ = 0.0f;
        nodeRoot->QueryFloatAttribute("scale", &scale);
        nodeRoot->QueryFloatAttribute("positionX", &positionX);
        nodeRoot->QueryFloatAttribute("positionY", &positionY);
        nodeRoot->QueryFloatAttribute("positionZ", &positionZ);
        nodeRoot->QueryFloatAttribute("rotX", &rotX);
        nodeRoot->QueryFloatAttribute("rotY", &rotY);
        nodeRoot->QueryFloatAttribute("rotZ", &rotZ);

        SceneNode* currentNode = new SceneNode(nodeName, {positionX, positionY, positionZ}, {rotX, rotY, rotZ}, scale);

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
                        const char* type = nullptr;

                        if (component->QueryFloatAttribute("zNear", &zNear) == tinyxml2::XML_SUCCESS) {
                            camera->setZNear(zNear);
                        }
                        if (component->QueryFloatAttribute("zFar", &zFar) == tinyxml2::XML_SUCCESS) {
                            camera->setZFar(zFar);
                        }

                        if (component->QueryStringAttribute("cameraType", &type) == tinyxml2::XML_SUCCESS) {
                            std::string cameraType = type;
                            if (cameraType == "FIRST_PERSON") {
                                camera->setType(CameraType::FIRST_PERSON);
                            } else if (cameraType == "THIRD_PERSON") {
                                camera->setType(CameraType::THIRD_PERSON);
                            } else if (cameraType == "ORTHOGRAPHIC") {
                                camera->setType(CameraType::ORTHOGRAPHIC);
                            } else if (cameraType == "ORTHOGRAPHIC_2D") {
                                camera->setType(CameraType::ORTHOGRAPHIC_2D);
                            } else {
                                VILLAIN_ERROR("Camera type {} not found", cameraType);
                            }
                        }

                        // NOTE: ATM every Camera as a NodeComponent is made main engine camera in addToEngine() method
                        // but probably this will change in the future and this flag will be needed
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

    void SceneParser::parseProperties(tinyxml2::XMLElement* e, SceneNode* rootNode) {

        for (tinyxml2::XMLElement *property = e->FirstChildElement(); property != NULL; property = property->NextSiblingElement()) {
            if (property->Value() == std::string("Skybox")) {
                std::vector<std::string> faces;
                faces.resize(6);
                for (tinyxml2::XMLElement *face = property->FirstChildElement(); face != NULL; face = face->NextSiblingElement()) {
                    if (face->Value() == std::string("Right"))
                        faces[0] = face->GetText();
                    if (face->Value() == std::string("Left"))
                        faces[1] = face->GetText();
                    if (face->Value() == std::string("Top"))
                        faces[2] = face->GetText();
                    if (face->Value() == std::string("Bottom"))
                        faces[3] = face->GetText();
                    if (face->Value() == std::string("Front"))
                        faces[4] = face->GetText();
                    if (face->Value() == std::string("Back"))
                        faces[5] = face->GetText();
                }
                rootNode->getEngine()->getRenderingEngine()->setSkybox(faces);
            }
        }
    }

}

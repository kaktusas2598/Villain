#include "SceneWriter.hpp"

#include "camera/Camera.hpp"

#include "Logger.hpp"

namespace Villain {

    void SceneWriter::saveSceneGraph(const std::string& fileName, SceneNode* rootNode) {
        tinyxml2::XMLDocument doc;

        VILLAIN_INFO("Saving scene graph: {}", fileName);

        // Creating root element
        tinyxml2::XMLElement* rootElement = doc.NewElement("Root");
        doc.InsertFirstChild(rootElement);

        // TODO: Save properties

        // Root element won't contain anything usually
        for (auto& n : rootNode->getChildren())
            saveSceneNode(doc, rootElement, n);

        doc.SaveFile(fileName.c_str());
    }

    void SceneWriter::saveSceneNode(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* parentElement, SceneNode* currentNode) {
        tinyxml2::XMLElement* nodeElement = doc.NewElement("Node");
        parentElement->InsertEndChild(nodeElement);

        nodeElement->SetAttribute("name", currentNode->getName().c_str());
        nodeElement->SetAttribute("positionX", currentNode->getTransform()->getPos().x);
        nodeElement->SetAttribute("positionY", currentNode->getTransform()->getPos().y);
        nodeElement->SetAttribute("positionZ", currentNode->getTransform()->getPos().z);
        nodeElement->SetAttribute("scale", currentNode->getTransform()->getScale());

        for (auto& c: currentNode->getComponents()) {
            // TODO:: Write component data:
            //        All Light types
            //        Move, Look controller
            //        RigidBody, Kinematic, Flight
            //        Mesh, Model renderers
            //        Script

            tinyxml2::XMLElement* componentElement = doc.NewElement("Component");
            nodeElement->InsertEndChild(componentElement);

            if (c->getID() == GetId<Camera>()) {
                Camera* camera = static_cast<Camera*>(c);
                componentElement->SetAttribute("type", "Camera");
                componentElement->SetAttribute("zNear", camera->getZnear());
                componentElement->SetAttribute("zFar", camera->getZfar());
                componentElement->SetAttribute("zoom", camera->getZoom());

                std::string cameraType = "NONE";
                switch (camera->getType()) {
                    case CameraType::FIRST_PERSON:
                        cameraType = "FIRST_PERSON";
                    case CameraType::THIRD_PERSON:
                        cameraType = "THIRD_PERSON";
                    case CameraType::ORTHOGRAPHIC:
                        cameraType = "ORTHOGRAPHIC";
                    case CameraType::ORTHOGRAPHIC_2D:
                        cameraType = "ORTHOGRAPHIC_2D";
                    default:
                        break;
                }
                tinyxml2::XMLElement* posElement = doc.NewElement("Component");
                componentElement->InsertEndChild(posElement);
                posElement->SetAttribute("positionX", camera->getPosition().x);
                posElement->SetAttribute("positionY", camera->getPosition().y);
                posElement->SetAttribute("positionZ", camera->getPosition().z);
            }
        }

        if (currentNode->getComponents().size()) {
            tinyxml2::XMLElement* childrenElement = doc.NewElement("Children");
            for (auto& c: currentNode->getChildren()) {
                saveSceneNode(doc, nodeElement, c);
            }
        }
    }
}

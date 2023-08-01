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
            // TODO: write data for each component attached to this node
            if (c->getID() == GetId<Camera>()) {
                Camera* camera = static_cast<Camera*>(c);
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

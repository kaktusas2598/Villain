#include "SceneParser.hpp"

#include "Engine.hpp"
#include "camera/Camera.hpp"
#include "Logger.hpp"
#include "SceneNode.hpp"
#include "components/KinematicController.hpp"
#include "components/Light.hpp"
#include "components/ModelRenderer.hpp"
#include "components/LookController.hpp"
#include "components/MoveController.hpp"
#include "components/ScriptComponent.hpp"
#include <sstream>

namespace Villain {

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

                    if (component->Attribute("type") == std::string("Camera")) {
                        Camera* camera = new Camera();
                        // Optional camera values
                        float zNear, zFar, zoom;
                        bool isMain = false;
                        const char* type = nullptr;

                        if (component->QueryFloatAttribute("zNear", &zNear) == tinyxml2::XML_SUCCESS) {
                            camera->setZNear(zNear);
                        }
                        if (component->QueryFloatAttribute("zFar", &zFar) == tinyxml2::XML_SUCCESS) {
                            camera->setZFar(zFar);
                        }
                        if (component->QueryFloatAttribute("zoom", &zoom) == tinyxml2::XML_SUCCESS) {
                            camera->setZoom(zoom);
                        }

                        camera->setPosition(parseVec3Element(component, "Position"));

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

                    if (component->Attribute("type") == std::string("DirectionalLight")) {
                        glm::vec3 ambient, diffuse, specular, direction;
                        ambient = parseColorAttribute(component, "Ambient", "color");
                        diffuse = parseColorAttribute(component, "Diffuse", "color");
                        specular = parseColorAttribute(component, "Specular", "color");
                        direction = parseVec3Element(component, "Direction");

                        DirectionalLight* light = new DirectionalLight(ambient, diffuse, specular, direction);
                        currentNode->addComponent(light);
                    }

                    if (component->Attribute("type") == std::string("PointLight")) {
                        glm::vec3 ambient, diffuse, specular, attenuation;
                        ambient = parseColorAttribute(component, "Ambient", "color");
                        diffuse = parseColorAttribute(component, "Diffuse", "color");
                        specular = parseColorAttribute(component, "Specular", "color");
                        attenuation = parseVec3Element(component, "Attenuation", "constant", "linear", "quadratic");

                        PointLight* light = new PointLight(ambient, diffuse, specular, {}, attenuation);
                        currentNode->addComponent(light);
                    }

                    if (component->Attribute("type") == std::string("SpotLight")) {
                        glm::vec3 ambient, diffuse, specular, attenuation, direction;
                        float cutOff, outerCutoff;
                        Camera* cam = nullptr;
                        ambient = parseColorAttribute(component, "Ambient", "color");
                        diffuse = parseColorAttribute(component, "Diffuse", "color");
                        specular = parseColorAttribute(component, "Specular", "color");
                        direction = parseVec3Element(component, "Direction");
                        attenuation = parseVec3Element(component, "Attenuation", "constant", "linear", "quadratic");
                        cutOff = parseFloatAttribute(component, "Cutoff", "angle");
                        outerCutoff = parseFloatAttribute(component, "OuterCutoff", "angle");

                        if (parseBoolAttribute(component, "Camera", "main")) {
                            // NOTE: Engine must have camera set at this point for xml to work!
                            cam = parentNode->getEngine()->getRenderingEngine()->getMainCamera();
                            VILLAIN_ASSERT(cam, "Cannot set flashlight as camera is not set yet");
                        }
                        SpotLight* light = new SpotLight(ambient, diffuse, specular,
                                currentNode->getTransform()->getPos(), direction, cutOff, outerCutoff, attenuation, cam);
                        currentNode->addComponent(light);
                    }

                    if (component->Attribute("type") == std::string("LookController")) {
                        currentNode->addComponent(new LookController());
                    }

                    if (component->Attribute("type") == std::string("MoveController")) {
                        currentNode->addComponent(new MoveController());
                    }

                    if (component->Attribute("type") == std::string("Model")) {
                        std::string modelPath = component->Attribute("fileName");
                        currentNode->addComponent(new ModelRenderer(new Model(modelPath.c_str())));
                    }



                    if (component->Attribute("type") == std::string("RigidBody")) {
                        RigidBody* rigidBody = new RigidBody();
                        CollisionPrimitive* colShape = nullptr;
                        bool kinematic = false;

                        component->QueryBoolAttribute("kinematic", &kinematic);

                        for (tinyxml2::XMLElement* e = component->FirstChildElement(); e != NULL; e = e->NextSiblingElement()) {
                            if (e->Value() == std::string("RigidBody")) {
                                float mass = 1.0f;
                                e->QueryFloatAttribute("mass", &mass);
                                rigidBody->setMass(10.0f);
                                rigidBody->setPosition(parseVec3Element(e, "Position"));

                                float linearDamping, angularDamping;
                                if (e->QueryFloatAttribute("linearDamping", &linearDamping) == tinyxml2::XML_SUCCESS)
                                    rigidBody->setLinearDamping(linearDamping);
                                if (e->QueryFloatAttribute("angularDamping", &angularDamping) == tinyxml2::XML_SUCCESS)
                                    rigidBody->setAngularDamping(angularDamping);
                            }
                            if (e->Value() == std::string("CollisionSphere")) {
                                float radius = 1.0f;
                                e->QueryFloatAttribute("radius", &radius);
                                colShape = new CollisionSphere(radius, rigidBody);
                            }
                            if (e->Value() == std::string("CollisionBox")) {
                                colShape = new CollisionBox(parseVec3Element(e, "HalfSize"), rigidBody);
                            }
                        }

                        if (kinematic) {
                            currentNode->addComponent(new KinematicController(rigidBody, colShape));
                        } else {
                            currentNode->addComponent(new RigidBodyComponent(rigidBody, colShape));
                        }
                    }

                    if (component->Attribute("type") == std::string("Script")) {
                        std::string modelPath = component->Attribute("fileName");
                        currentNode->addComponent(new ScriptComponent(modelPath));
                    }

                    // TODO: particle system component
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

            if (property->Value() == std::string("Ambient")) {
                glm::vec3 ambientColor = parseColorAttribute(e, "Ambient", "color");
                rootNode->getEngine()->getRenderingEngine()->setAmbientLightColor(ambientColor);
            }
        }
    }

    std::string SceneParser::GetUniqueId() {
       static int n = 1;
       std::ostringstream os;
       os << n++;
       return os.str();
    }

    glm::vec3 SceneParser::hexToVec3(const std::string& hexColor) {
        // Remove the '#' character from the hex color string
        std::string colorString = hexColor.substr(1);

        // Parse the individual color components from the hex string
        int r, g, b;
        std::istringstream(colorString.substr(0, 2)) >> std::hex >> r;
        std::istringstream(colorString.substr(2, 2)) >> std::hex >> g;
        std::istringstream(colorString.substr(4, 2)) >> std::hex >> b;

        // Normalize the color values to the range [0, 1]
        glm::vec3 color(r / 255.0f, g / 255.0f, b / 255.0f);

        return color;
    }

    glm::vec3 SceneParser::parseColorAttribute(tinyxml2::XMLElement* parentElement, const char* childElementName, const char* attributeName) {
        tinyxml2::XMLElement* childElement = parentElement->FirstChildElement(childElementName);
        if (childElement) {
            const char* color = nullptr;
            if (childElement->QueryStringAttribute(attributeName, &color) == tinyxml2::XML_SUCCESS) {
                return hexToVec3(color);
            }
        }
        return glm::vec3(0.0f);
    }

    glm::vec3 SceneParser::parseVec3Element(tinyxml2::XMLElement* element, const std::string& elementName,
            const std::string& x, const std::string& y, const std::string& z) {
        glm::vec3 vec3Value;

        tinyxml2::XMLElement* vec3Element = element->FirstChildElement(elementName.c_str());
        if (vec3Element) {
            vec3Value.x = vec3Element->FloatAttribute(x.c_str());
            vec3Value.y = vec3Element->FloatAttribute(y.c_str());
            vec3Value.z = vec3Element->FloatAttribute(z.c_str());
        }

        return vec3Value;
    }

    float SceneParser::parseFloatAttribute(tinyxml2::XMLElement* parentElement, const char* childElementName, const char* attribName) {
        tinyxml2::XMLElement* childElement = parentElement->FirstChildElement(childElementName);
        float value = 0.0f;
        if (childElement) {
            childElement->QueryFloatAttribute(attribName, &value);
        }

        return value;
    }

    bool SceneParser::parseBoolAttribute(tinyxml2::XMLElement* parentElement, const char* childElementName, const char* attribName) {
        tinyxml2::XMLElement* childElement = parentElement->FirstChildElement(childElementName);
        bool value = false;
        if (childElement) {
            childElement->QueryBoolAttribute(attribName, &value);
        }

        return value;
    }
}

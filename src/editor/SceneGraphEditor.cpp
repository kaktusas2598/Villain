#include "SceneGraphEditor.hpp"


#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <imgui/imgui.h>

#include "Application.hpp"
#include "Engine.hpp"
#include "components/KinematicController.hpp"
#include "components/Light.hpp"
#include "components/LookController.hpp"
#include "components/MeshRenderer.hpp"
#include "components/ModelRenderer.hpp"
#include "components/MoveController.hpp"
#include "components/ScriptComponent.hpp"
#include "rendering/MeshUtils.hpp"

namespace Villain {

    SceneGraphEditor::SceneGraphEditor(ImGuiLayer* editor) : editor(editor) {}

    void SceneGraphEditor::init() {
        editor->getEngine()->getEventDispatcher()->registerCallback(BIND_EVENT_FN(onEvent));
    }

    void SceneGraphEditor::onEvent(Event& event) {
        if (event.getType() == EventType::FileSelectedEvent) {
            FileSelectedEvent e = static_cast<FileSelectedEvent&>(event);
            // Check if we're loading a model, when load it for selected node
            if (selectedNode) {
                // TODO: need probably an enum FileType or AssetType or sth to avoid this looping
                for (const std::string& supportedFormat : supportedModelFormats) {
                    if (e.getExtension() == supportedFormat) {
                        selectedNode->addComponent(new ModelRenderer(ResourceManager::Instance()->loadModel(e.getFileName(), e.getFileName())));
                    }
                }
                for (const std::string& supportedFormat : supportedTextureFormats) {
                    if (e.getExtension() == supportedFormat) {
                        // TODO: Consider using other data types to store components in node for faster access of specific component
                        for (auto& c : selectedNode->getComponents())
                            if (c->getID() == GetId<MeshRenderer<VertexP1N1UV>>()) {
                                // TODO: how to know if we want to set diffuse vs specular etc.. ?
                                auto meshN1UV = static_cast<MeshRenderer<VertexP1N1UV>*>(c);
                                std::string diffuseMapName = c->getParent()->getName() + std::string(" diffuse map");
                                meshN1UV->getMaterial().setDiffuseMap(ResourceManager::Instance()->loadTexture(e.getFileName(), diffuseMapName));
                            }
                    }
                }
                // Load script for selected node
                if (e.getExtension() == ".lua") {
                    selectedNode->addComponent(new ScriptComponent(e.getRootPath() + e.getRelativePath()));
                }
            }

            // Open scene graph xml file
            if (e.getExtension() == ".xml") {
                editor->getEngine()->getApplication()->loadScene(e.getFileName());
            }
        }
    }

    void SceneGraphEditor::render(Engine& engine) {
        // Render the whole hierarchy
        ImGui::Begin("Scene Graph");
        {
            ImGui::SetNextItemOpen(true);
            ImGui::Unindent(ImGui::GetTreeNodeToLabelSpacing());
            drawNodeHierarchy(engine.getApplication()->getRootNode());
            ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.3f, 1.0f));
            if (ImGui::Button("New Node")) {
                SceneNode* newNode = new SceneNode("New Node");
                engine.getApplication()->getRootNode()->addChild(newNode);
                selectedNode = newNode;
                shouldSetFocus = true;
            }
            ImGui::PopStyleColor();
        }
        ImGui::End();

        // Only render selected graph node
        drawSelectedNode();
    }

    void SceneGraphEditor::drawNodeHierarchy(SceneNode* node) {
        // Make sure tree nods fill all available width and disable single click to open behaviour
        ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnDoubleClick;
        if (selectedNode == node) {
            nodeFlags |= ImGuiTreeNodeFlags_Selected;
        }

        bool nodeOpen =  ImGui::TreeNodeEx(node->getName().c_str(), nodeFlags);
        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
            selectedNode = node;
            // Set id in rendering engine so that color mod is applied to shader when drawing selected node
            node->getEngine()->getRenderingEngine()->setSelectedNodeID(selectedNode->getID());
            shouldSetFocus = true;
        }
        if (nodeOpen) {
            if (ImGui::BeginTabBar("NodeProps", ImGuiTabBarFlags_None)) {
                if (ImGui::BeginTabItem("Transform")) {
                    drawNodeProperties(node);
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Components")) {
                    drawNodeComponents(node);
                    ImGui::EndTabItem();
                }

                ImGui::EndTabBar();
            }
            // Children
            for (auto& child: node->getChildren()) {
                drawNodeHierarchy(child);
            }

            ImGui::TreePop();
        }
    }

    void SceneGraphEditor::drawSelectedNode() {
        if (shouldSetFocus) {
            ImGui::SetNextWindowFocus();
            shouldSetFocus = false;
        }
        ImGui::Begin("Active Node");

        if (selectedNode) {
            //ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, )
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.1f, 1.0f));
            if (ImGui::Button("Remove Node")) {
                editor->getEngine()->getApplication()->deleteNode(selectedNode);
                selectedNode = nullptr;
                return;
            }
            ImGui::PopStyleColor();

            editor->separatorHeader("Properties", 1.15);

            char nodeNameBuffer[100];
            std::strcpy(nodeNameBuffer, selectedNode->getName().c_str());
            if (ImGui::InputText("Node Name", nodeNameBuffer, sizeof(nodeNameBuffer))) {
                selectedNode->setName(nodeNameBuffer);
            }

            drawNodeProperties(selectedNode);

            editor->separatorHeader("Add new components", 1.15);
            // Add component combo menu
            if (ImGui::BeginCombo("Add Component", componentNames[selectedComponent])) {
                for (const auto& entry : componentNames) {
                    ComponentType componentType = entry.first;
                    bool isSelected = (selectedComponent == componentType);
                    if (ImGui::Selectable(entry.second, isSelected)) {
                        selectedComponent = componentType;
                    }
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            std::vector<VertexP1N1UV> vertices;
            std::vector<unsigned int> indices;
            glm::vec3 diffuseColor = glm::vec3(1.0f);
            glm::vec3 lightDirection = glm::vec3(0.0f);
            float spotLightCutoff = 12.5f;
            float spotLightOuterCutoff = 17.5f;

            // Display component creation info depending on selection in the listbox
            switch (selectedComponent) {
                case ComponentType::Camera:
                    if (ImGui::Button("Add camera")) {
                        // TODO: need to be able to set main camera whenever we want to, not automatically
                        selectedNode->addComponent(new Camera());
                    }
                    break;
                case ComponentType::Light:
                    ImGui::ColorEdit3("Diffuse light ", (float*)&diffuseColor);
                    ImGui::SliderFloat3("Direction (for spot and directional lights only)", (float*)&lightDirection, -1.f, 1.f);
                    ImGui::DragFloat("Spot light cutoff angle", &spotLightCutoff, 1.0f, 0.0f, 180.0f);
                    ImGui::DragFloat("Spot light outer cutoff angle", &spotLightOuterCutoff, 1.0f, 0.0f, 180.0f);

                    if (ImGui::Button("Add Point Light")) {
                        selectedNode->addComponent(
                                new PointLight(diffuseColor * glm::vec3(0.2f), diffuseColor, glm::vec3(1.0f),
                                    selectedNode->getTransform()->getPos()));
                    } ImGui:: SameLine();
                    if (ImGui::Button("Add Spot Light")) {
                        selectedNode->addComponent(
                                new SpotLight(diffuseColor * glm::vec3(0.2f), diffuseColor, glm::vec3(1.0f),
                                    selectedNode->getTransform()->getPos(), lightDirection,
                                    glm::cos(glm::radians(spotLightCutoff)), glm::cos(glm::radians(spotLightOuterCutoff))));

                    } ImGui:: SameLine();
                    if (ImGui::Button("Add Directional Light")) {
                        selectedNode->addComponent(
                                new DirectionalLight(diffuseColor * glm::vec3(0.2f), diffuseColor, glm::vec3(1.0f),
                                    lightDirection));
                    } ImGui:: SameLine();

                    break;
                case ComponentType::Mesh:
                    if (ImGui::Button("Add sphere mesh")) {
                        MeshUtils<VertexP1N1UV>::addSphere(&vertices, &indices, 1.0f);
                        selectedNode->addComponent(new MeshRenderer<VertexP1N1UV>(new Mesh<VertexP1N1UV>(vertices, indices), Material()));
                    }
                    if (ImGui::Button("Add Axis-Aligned Bounding Box mesh")) {
                        MeshUtils<VertexP1N1UV>::addAABB(&vertices, &indices);
                        selectedNode->addComponent(new MeshRenderer<VertexP1N1UV>(new Mesh<VertexP1N1UV>(vertices, indices), Material()));
                    }
                    break;
                case ComponentType::Model:
                    if (ImGui::Button("Add Model")) {
                        editor->getFileBrowser().openPopup();
                    }
                    break;
                case ComponentType::MoveController:
                    if (ImGui::Button("Add MoveController")) {
                        selectedNode->addComponent(new MoveController());
                    }
                    break;
                case ComponentType::LookController:
                    if (ImGui::Button("Add LookController")) {
                        selectedNode->addComponent(new LookController());
                    }
                    break;
                case ComponentType::RigidBody:
                    static int colliderType = 0;
                    static float mass = 1.0f;
                    static bool kinematic = false;

                    ImGui::Text("Properties");
                    ImGui::DragFloat("Mass", &mass, 0.1f, 0.0f, 1000.0f, "%.1f");
                    ImGui::Checkbox("Kinematic", &kinematic);

                    ImGui::Text("Collision primitive");
                    ImGui::RadioButton("NONE", &colliderType, 0); ImGui::SameLine();
                    ImGui::RadioButton("SPHERE", &colliderType, 1); ImGui::SameLine();
                    ImGui::RadioButton("BOX", &colliderType, 2);

                    static float radius = 1.0f;
                    static glm::vec3 boxHalfSize{0.5, 0.5, 0.5};
                    ImGui::DragFloat("Sphere radius", &radius, 0.1f, 0.0f, 100.0f, "%.1f");
                    ImGui::DragFloat3("Box half-size", glm::value_ptr(boxHalfSize), 0.1f, 0.0f, 100.0f, "%.1f");

                    if (ImGui::Button("Add Rigid Body")) {
                        RigidBody* body = new RigidBody();
                        body->setPosition(selectedNode->getTransform()->getPos());
                        body->setOrientation(selectedNode->getTransform()->getOriantation());
                        body->setMass(mass);
                        CollisionPrimitive* col = nullptr;
                        if (colliderType == 1) {
                            col = new CollisionSphere(radius, body);
                        } else if (colliderType == 2) {
                            col = new CollisionBox(boxHalfSize, body);
                        }
                        if (kinematic)
                            selectedNode->addComponent(new KinematicController(body, col));
                        else
                            selectedNode->addComponent(new RigidBodyComponent(body, col));
                    }
                    break;
                case ComponentType::Script:
                    if (ImGui::Button("Add Script")) {
                        editor->getFileBrowser().openPopup();
                    }
                    break;
                case ComponentType::None:
                    break;
                default:
                    VILLAIN_WARN("Option {} not implemented", componentNames[selectedComponent]);
                    break;
            }

            editor->separatorHeader("Components", 1.15);
            // Draw existing components
            if (!selectedNode->getComponents().empty()) {
                drawNodeComponents(selectedNode);
            }
        } else {
            ImGui::Text("Select a node or create a new node in the scene graph");
        }

        ImGui::End();
    }

    void SceneGraphEditor::drawNodeProperties(SceneNode* node) {
        ImGui::DragFloat("Scale", node->getTransform()->getScalePtr(), 1.0f, 0.0f, 10.0f);

        ImGui::Text("Position"); ImGui::SameLine();
        ImGui::PushItemWidth(40);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor(0.8f, 0.1f, 0.1f));
        ImGui::DragFloat("X", &node->getTransform()->getPos().x); ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor(0.2f, 0.7f, 0.3f));
        ImGui::DragFloat("Y", &node->getTransform()->getPos().y); ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor(0.1f, 0.0f, 0.9f));
        ImGui::DragFloat("Z", &node->getTransform()->getPos().z);
        ImGui::PopStyleColor(3);
        ImGui::PopItemWidth();

        ImGui::SliderFloat3("Rot", (float*)&node->getTransform()->getEulerRot(), -360.f, 360.f);
    }

    void SceneGraphEditor::drawNodeComponents(SceneNode* node) {
        if (!node->getComponents().empty()) {
            int i = 0;
            for (auto& compo: node->getComponents()) {
                std::string removeBtnName = "Remove Component " + std::to_string(compo->getID());
                if (ImGui::Button(removeBtnName.c_str())) {
                    selectedNode->removeComponent(compo);
                    continue;
                }

                ImGui::PushID(i); // Solves issues with multiple elements sharing same names
                if (compo->getID() == GetId<Camera>()) {
                    editor->renderIcon("\uf030"); ImGui::SameLine();
                    ImGui::Text("Camera");
                    Camera* camera = static_cast<Camera*>(compo);

                    ImGui::DragFloat("Z near plane", camera->getZnearPtr());
                    ImGui::DragFloat("Z far plane", camera->getZfarPtr());
                    ImGui::DragFloat("Zoom", camera->getZoomPtr());
                    ImGui::DragFloat("Distance to target (third person only)", camera->getDistanceToTargetPtr());

                    std::map<CameraType, const char*> types;
                    types[CameraType::NONE] = "None";
                    types[CameraType::ORTHOGRAPHIC_2D] = "2D";
                    types[CameraType::ORTHOGRAPHIC] = "Orthographic";
                    types[CameraType::FIRST_PERSON] = "Perspective/First Person";
                    types[CameraType::THIRD_PERSON] = "Third Person";
                    if (ImGui::BeginCombo("Camera Type", types[camera->getType()])) {
                        for (const auto& type: types)  {
                            bool isSelected = (type.first == camera->getType());
                            if (ImGui::Selectable(type.second, isSelected)) {
                                camera->setType(type.first);
                            }

                            if (isSelected) {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }
                }

                if (auto light = dynamic_cast<BaseLight*>(compo)) {
                    editor->renderIcon("\uf0eb"); ImGui::SameLine();
                    ImGui::Text("%s light", light->type().c_str());
                    ImGui::DragFloat("Shadow Bias", light->getShadowInfo()->getBiasPointer());
                    ImGui::ColorEdit3("Ambient light color", (float*)&light->AmbientColor, ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float);
                    ImGui::ColorEdit3("Diffuse light color", (float*)&light->DiffuseColor, ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float);
                    ImGui::ColorEdit3("Specular light color", (float*)&light->SpecularColor, ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float);
                    // HACK: seems like I do this everywhere anyway, so maybe separate properties for specular and ambient colours of light sources
                    // are not needed? yet to be decided
                    //light->AmbientColor = light->DiffuseColor * 0.2f;
                }

                if (compo->getID() == GetId<MeshRenderer<VertexP1N1UV>>()) {
                    editor->renderIcon("\uf61f"); ImGui::SameLine();
                    ImGui::Text("Basic Mesh P1N1UV");
                    auto meshN1UV = static_cast<MeshRenderer<VertexP1N1UV>*>(compo);
                    auto material = meshN1UV->getMaterial();

                    ImGui::ColorEdit4("Ambient color", meshN1UV->getMaterial().getAmbientColorPtr());
                    ImGui::ColorEdit4("Diffuse color", meshN1UV->getMaterial().getDiffuseColorPtr());
                    ImGui::ColorEdit4("Specular color", meshN1UV->getMaterial().getSpecularColorPtr());
                    ImGui::DragFloat("Specular factor", meshN1UV->getMaterial().getSpecularFactorPtr());

                    if (!material.getDiffuseMap()) {
                        if (ImGui::Button("Load diffuse map")) {
                            editor->getFileBrowser().openPopup();
                        }

                    }

                    static std::string selectedTexture;
                    if (ImGui::BeginCombo("Diffuse Map", selectedTexture.c_str())) {
                        bool noMap = meshN1UV->getMaterial().getDiffuseMap() == nullptr;
                        if (ImGui::Selectable("--- None---", noMap)) {
                            meshN1UV->getMaterial().setDiffuseMap(nullptr);
                            selectedTexture = std::string();
                        }
                        if (noMap) {
                            ImGui::SetItemDefaultFocus();
                        }

                        for (const auto& texturePair : ResourceManager::Instance()->getTextureMap()) {
                            const std::string& textureName = texturePair.first;
                            const Texture* texture = texturePair.second;

                            // Pass nullptr as the second parameter to use the text in the map as the ID
                            bool isSelected = (meshN1UV->getMaterial().getDiffuseMap() == texture);
                            if (ImGui::Selectable(textureName.c_str(), isSelected)) {
                                // Update the selected texture when an option is selected
                                meshN1UV->getMaterial().setDiffuseMap(const_cast<Texture*>(texture));
                            }
                            if (isSelected) {
                                // Set the initial focus on the currently selected texture
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }

                    static int loadedMesh = 0;
                    std::vector<VertexP1N1UV> vertices;
                    std::vector<unsigned int> indices;

                    ImGui::RadioButton("DEFAULT", &loadedMesh, 0); ImGui::SameLine();
                    ImGui::RadioButton("SPHERE", &loadedMesh, 1); ImGui::SameLine();
                    ImGui::RadioButton("AABB", &loadedMesh, 2);

                    static float sphereRadius = 1.0f;
                    static glm::vec3 aabbSize = glm::vec3(1.0f);
                    ImGui::DragFloat("Sphere radius", &sphereRadius, 1.0f, 0.0f, 100.0f, "%.1f");
                    ImGui::DragFloat3("AABB size", glm::value_ptr(aabbSize), 1.0f, 0.0f, 100.0f, "%.1f");

                    if (loadedMesh == 1) {
                        MeshUtils<VertexP1N1UV>::addSphere(&vertices, &indices, sphereRadius);
                        meshN1UV->setMesh(new Mesh<VertexP1N1UV>(vertices, indices));
                    } else if (loadedMesh == 2) {
                        MeshUtils<VertexP1N1UV>::addAABB(&vertices, &indices, glm::vec3(0.0f), aabbSize/2.0f);
                        meshN1UV->setMesh(new Mesh<VertexP1N1UV>(vertices, indices));
                    }
                }

                if (compo->getID() == GetId<ModelRenderer>()) {
                    editor->renderIcon("\uf6c8"); ImGui::SameLine();
                    auto model = static_cast<ModelRenderer*>(compo);
                    ImGui::Text("Model %s at %s", model->getModel()->getFilename().c_str(), model->getModel()->getDirectory().c_str());
                    if (model->getCurrentAnimation()) {
                        if (ImGui::BeginCombo("Active animation", model->getCurrentAnimation()->getName().c_str())) {
                            for (const auto& anim: model->getModel()->getAnimations())  {
                                bool isSelected = (anim.first == model->getCurrentAnimation()->getName());
                                if (ImGui::Selectable(anim.first.c_str(), isSelected)) {
                                    model->getAnimator()->playAnimation(anim.second);
                                }

                                if (isSelected) {
                                    ImGui::SetItemDefaultFocus();
                                }
                            }

                            ImGui::EndCombo();
                        }
                        ImGui::DragFloat("Animation speed", model->getCurrentAnimation()->getSpeed(), 1.0f, 0.1f, 10000.0f, "%.1f");
                        ImGui::DragFloat("Animation time", model->getAnimator()->getCurrentTime(), 1.0f, 0.0f, model->getCurrentAnimation()->getDuration(), "%.1f");
                        ImGui::Checkbox("Bind Pose", model->getAnimator()->getBindPose());
                        for(auto& boneInfo: model->getModel()->getBoneInfoMap()) {
                            if (boneInfo.second.id == model->getModel()->getDisplayedBoneIndex()) {
                                ImGui::Text("Displayed Bone: '%s'", boneInfo.first.c_str());
                            }
                        }
                        if (ImGui::Button("Pause Animation")) {
                            model->getAnimator()->toggleAnimation();
                        }

                    }
                }

                if (compo->getID() == GetId<RigidBodyComponent>() || compo->getID() == GetId<KinematicController>()) {
                    editor->renderIcon("\uf1b9"); ImGui::SameLine();
                    auto rg = dynamic_cast<RigidBodyComponent*>(compo);
                    if (compo->getID() == GetId<KinematicController>()) {
                        //rg = dynamic_cast<KinematicController*>(compo);
                        ImGui::Text("Kinematic Rigid Body");
                    } else {
                        ImGui::Text("Rigid Body");
                    }

                    ImGui::Text("Mass: %f", rg->getBody()->getMass());
                    ImGui::Text("Position: %s", glm::to_string(rg->getBody()->getPosition()).c_str());
                    ImGui::Text("Orientation: %s", glm::to_string(rg->getBody()->getOrientation()).c_str());
                    ImGui::Text("Linear Velocity: %s", glm::to_string(rg->getBody()->getLinearVelocity()).c_str());
                    ImGui::Text("Angular Velocity: %s", glm::to_string(rg->getBody()->getAngularVelocity()).c_str());
                    if (rg->getCollider()) {
                        if (auto box = dynamic_cast<CollisionBox*>(rg->getCollider())) {
                            ImGui::DragFloat3("Collider Box Half Size", glm::value_ptr(box->halfSize), 0.1f);
                        }
                        if (auto sphere = dynamic_cast<CollisionSphere*>(rg->getCollider())) {
                            ImGui::DragFloat("Collider Sphere Radius", &sphere->radius, 0.1f);
                        }
                    }
                }

                if (compo->getID() == GetId<LookController>()) {
                    editor->renderIcon("\uf06e"); ImGui::SameLine();
                    ImGui::Text("Look Controller");
                    auto control = static_cast<LookController*>(compo);
                    ImGui::DragFloat("Mouse sensitivity", control->getSensitivity());
                    ImGui::Checkbox("Constrain Pitch", control->getConstrainPitch());
                }

                if (compo->getID() == GetId<MoveController>()) {
                    editor->renderIcon("\uf554"); ImGui::SameLine();
                    ImGui::Text("Move Controller");
                    auto control = static_cast<MoveController*>(compo);
                    ImGui::DragFloat("Movement speed", control->getSpeed());
                }

                if (compo->getID() == GetId<ScriptComponent>()) {
                    auto script = static_cast<ScriptComponent*>(compo);
                    editor->renderIcon("\uf186"); ImGui::SameLine();
                    ImGui::Text("Script: %s", script->getScript().getFilename().c_str());
                    if (ImGui::Button("Edit"))
                        editor->getScriptEditor().setScript(script->getScript().getFilename());
                }

                ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
                ImGui::PushStyleVar(ImGuiStyleVar_SeparatorTextBorderSize, 2.0f);
                ImGui::Separator();
                ImGui::PopStyleColor();
                ImGui::PopStyleVar();
                ImGui::PopID();
                i++;
            }
        }
    }
}

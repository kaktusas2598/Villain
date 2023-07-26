#include "SceneGraphEditor.hpp"


#include "imgui/imgui.h"

#include "Application.hpp"
#include "Engine.hpp"
#include "components/KinematicController.hpp"
#include "components/Light.hpp"
#include "components/LookController.hpp"
#include "components/MeshRenderer.hpp"
#include "components/ModelRenderer.hpp"
#include "components/MoveController.hpp"
#include "rendering/MeshUtils.hpp"

namespace Villain {

    void SceneGraphEditor::render(Engine& engine) {
        // Render the whole hierarchy
        ImGui::Begin("Scene Graph");
        {
            ImGui::SetNextItemOpen(true);
            ImGui::Unindent(ImGui::GetTreeNodeToLabelSpacing());
            drawNodeHierarchy(engine.getApplication()->getRootNode());
            ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());

            if (ImGui::Button("New Node")) {
                SceneNode* newNode = new SceneNode("testing");
                engine.getApplication()->getRootNode()->addChild(newNode);
            }
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
        ImGui::Begin("Active Node");

        if (selectedNode != nullptr) {
            ImGui::Text("%s", selectedNode->getName().c_str());

            drawNodeProperties(selectedNode);

            if (!selectedNode->getComponents().empty()) {
                drawNodeComponents(selectedNode);
            }


            ImGui::Separator();
            const char* componentList[] = {"Camera", "Light", "Basic Mesh", "Model", "Move Controller", "Look Controller", "Physics"};
            static int selectedComponent = 1;
            // NOTE: wrap ListBox() in if condition to check for click events on list items
            ImGui::ListBox("Add Component", &selectedComponent, componentList, IM_ARRAYSIZE(componentList), 4);

            std::vector<VertexP1N1UV> vertices;
            std::vector<unsigned int> indices;
            glm::vec3 diffuseColor = glm::vec3(1.0f);
            glm::vec3 lightDirection = glm::vec3(0.0f);
            float spotLightCutoff = 12.5f;
            float spotLightOuterCutoff = 17.5f;

            // Display component creation info depending on selection in the listbox
            switch (selectedComponent) {
                    case 0:
                        if (ImGui::Button("Add camera")) {
                            // TODO: need to be able to set main camera whenever we want to, not automatically
                            selectedNode->addComponent(new Camera());
                        }
                        break;
                    case 1:
                        ImGui::ColorEdit3("Diffuse light ", (float*)&diffuseColor);
                        ImGui::SliderFloat3("Direction (for spot and directional lights only)", (float*)&lightDirection, -1.f, 1.f);
                        ImGui::DragFloat("Spot light cutoff angle", &spotLightCutoff, 1.0f, 0.0f, 180.0f);
                        ImGui::DragFloat("Spot light outer cutoff angle", &spotLightOuterCutoff, 1.0f, 0.0f, 180.0f);
                        ImGui::Separator();

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
                    case 2:
                        // TODO:
                        if (ImGui::Button("Add sphere mesh")) {
                            MeshUtils<VertexP1N1UV>::addSphere(&vertices, &indices, 1.0f);
                            selectedNode->addComponent(new MeshRenderer<VertexP1N1UV>(new Mesh<VertexP1N1UV>(vertices, indices), Material()));
                        }
                        if (ImGui::Button("Add Axis-Aligned Bounding Box mesh")) {
                            MeshUtils<VertexP1N1UV>::addAABB(&vertices, &indices);
                            selectedNode->addComponent(new MeshRenderer<VertexP1N1UV>(new Mesh<VertexP1N1UV>(vertices, indices), Material()));
                        }
                        break;
                    case 3:
                        // TODO: need a way to select model, possibly using C++'s filesystem header
                        //selectedNode->addComponent(new ModelRenderer(""))
                        break;
                    default:
                        printf("Option %s not implemented\n", componentList[selectedComponent]);
                        break;
                }

        }

        ImGui::End();
    }

    void SceneGraphEditor::drawNodeProperties(SceneNode* node) {
        ImGui::DragFloat("Scale", node->getTransform()->getScalePtr(), 1.0f, 0.0f, 10.0f);

        ImGui::Text("Position"); ImGui::SameLine();
        ImGui::PushItemWidth(40);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor(1.0f, 0.0f, 0.0f));
        ImGui::DragFloat("X", &node->getTransform()->getPos().x); ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor(0.0f, 1.0f, 0.0f));
        ImGui::DragFloat("Y", &node->getTransform()->getPos().y); ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor(0.0f, 0.0f, 1.0f));
        ImGui::DragFloat("Z", &node->getTransform()->getPos().z);
        ImGui::PopStyleColor(3);
        ImGui::PopItemWidth();

        ImGui::SliderFloat3("Rot", (float*)&node->getTransform()->getEulerRot(), -360.f, 360.f);
    }

    void SceneGraphEditor::drawNodeComponents(SceneNode* node) {
        if (!node->getComponents().empty()) {
            int i = 0;
            for (auto& compo: node->getComponents()) {
                ImGui::PushID(i); // Solves issues with multiple elements sharing same names
                // TODO: find optimal way of adding any components and possibly custom components without too
                // many conditionals
                if (compo->getID() == GetId<Camera>()) {
                    ImGui::Text("Camera");
                    Camera* camera = static_cast<Camera*>(compo);

                    ImGui::Text("Zoom: %.1f", camera->getZoom());

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
                auto light = dynamic_cast<BaseLight*>(compo);
                if (light != nullptr) {
                    ImGui::Text("%s light", light->type().c_str());
                    ImGui::DragFloat("Shadow Bias", light->getShadowInfo()->getBiasPointer());
                    ImGui::ColorEdit3("Diffuse light color", (float*)&light->DiffuseColor);
                    // HACK: seems like I do this everywhere anyway, so maybe separate properties for specular and ambient colours of light sources
                    // are not needed? yet to be decided
                    light->AmbientColor = light->DiffuseColor * 0.2f;
                }

                auto meshN1UV = dynamic_cast<MeshRenderer<VertexP1N1UV>*>(compo);
                if (meshN1UV != nullptr) {
                    ImGui::Text("Basic Mesh");
                    static int loadedMesh = 0;
                    auto originalMesh = meshN1UV->getMesh();
                    std::vector<VertexP1N1UV> vertices;
                    std::vector<unsigned int> indices;

                    ImGui::RadioButton("DEFAULT", &loadedMesh, 0); ImGui::SameLine();
                    ImGui::RadioButton("SPHERE", &loadedMesh, 1); ImGui::SameLine();
                    ImGui::RadioButton("AABB", &loadedMesh, 2);

                    static float sphereRadius = 1.0f;
                    static float aabbSize = 1.0f;;
                    ImGui::DragFloat("Sphere radius", &sphereRadius, 1.0f, 0.0f, 100.0f, "%.1f");
                    ImGui::DragFloat("AABB size", &aabbSize, 1.0f, 0.0f, 100.0f, "%.1f");

                    if (loadedMesh == 0)
                        meshN1UV->setMesh(originalMesh);
                    else if (loadedMesh == 1) {
                        MeshUtils<VertexP1N1UV>::addSphere(&vertices, &indices, sphereRadius);
                        meshN1UV->setMesh(new Mesh<VertexP1N1UV>(vertices, indices));
                    } else if (loadedMesh == 2) {
                        MeshUtils<VertexP1N1UV>::addAABB(&vertices, &indices, glm::vec3(0.0f), glm::vec3(aabbSize/2));
                        meshN1UV->setMesh(new Mesh<VertexP1N1UV>(vertices, indices));
                    }
                }

                auto model = dynamic_cast<ModelRenderer*>(compo);
                if (model != nullptr) {
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

                ImGui::Separator();
                ImGui::PopID();
                i++;
            }
        }
    }

}

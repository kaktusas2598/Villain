#include "AssetBrowser.hpp"

#include "audio/AudioSource.hpp"
#include "editor/ImGuiLayer.hpp"
#include "imgui/imgui.h"
#include "ResourceManager.hpp"
#include "rendering/PBRMaterial.hpp"

namespace Villain {

    void AssetBrowser::render() {
        ImGui::Begin("Asset Browser");

        editor->renderIcon("\uf001"); ImGui::SameLine();
        if (ImGui::TreeNode("Audio")) {
            for (auto const& t: ResourceManager::Instance()->getAudioMap()) {
                // NOTE: Not sure why using tree node correct sound was playing only when closing the node
                if (ImGui::Selectable(t.first.c_str())) {
                    AudioSource source(t.second);
                    source.play();
                }
            }

            ImGui::TreePop();
        }

        editor->renderIcon("\uf5aa"); ImGui::SameLine();
        if (ImGui::TreeNode("Materials")) {
            for (auto const& t: ResourceManager::Instance()->getMaterialMap()) {
                if (ImGui::TreeNode(t.first.c_str())) {
                    renderMaterial(t.second);
                    ImGui::TreePop();
                }
            }

            ImGui::TreePop();
        }

        editor->renderIcon("\uf1b3"); ImGui::SameLine();
        if (ImGui::TreeNode("Models")) {
            for (auto const& t: ResourceManager::Instance()->getModelMap()) {
                if (ImGui::TreeNode(t.first.c_str())) {
                    ImGui::TreePop();
                }
            }

            ImGui::TreePop();
        }

        editor->renderIcon("\uf0ad"); ImGui::SameLine();
        if (ImGui::TreeNode("Shaders")) {
            for (auto const& t: ResourceManager::Instance()->getShaderMap()) {
                if (ImGui::TreeNode(t.first.c_str())) {
                    ImGui::TreePop();
                }
            }

            ImGui::TreePop();
        }

        editor->renderIcon("\uf1fc"); ImGui::SameLine();
        if (ImGui::TreeNode("Textures")) {
            ImVec2 desiredImgSize = ImGui::GetContentRegionAvail(); // will squish image to fit in
            for (auto const& t: ResourceManager::Instance()->getTextureMap()) {
                if (ImGui::TreeNode(t.first.c_str())) {
                    ImGui::Image(
                            reinterpret_cast<ImTextureID>(t.second->getID()),
                            //ImGui::GetWindowSize(), // will respect aspect ratio of image
                            desiredImgSize,
                            ImVec2(0, 1),
                            ImVec2(1, 0)
                            );

                    ImGui::TreePop();
                }
            }

            ImGui::TreePop();
        }

        editor->renderIcon("\uf279"); ImGui::SameLine();
        if (ImGui::TreeNode("Environment(HDR) Maps")) {
            ImVec2 desiredImgSize = ImGui::GetContentRegionAvail(); // will squish image to fit in
            for (auto const& t: ResourceManager::Instance()->getHDRMap()) {
                if (ImGui::TreeNode(t.first.c_str())) {
                    ImGui::Image(
                            reinterpret_cast<ImTextureID>(t.second->getHDRTextureId()),
                            desiredImgSize,
                            ImVec2(0, 1),
                            ImVec2(1, 0)
                            );

                    ImGui::TreePop();
                }
            }

            ImGui::TreePop();
        }

        // TODO: fonts, levels

        ImGui::TreePop();

        ImGui::End();
    }

    void AssetBrowser::renderMaterial(Material* material) {
        ImGui::Text("Basic Mesh P1N1T1B1UV");

        if (auto pbr = dynamic_cast<PBRMaterial*>(material)) {
            ImGui::ColorEdit3("Albedo", pbr->getAlbedoColorPtr());
            ImGui::DragFloat("Roughness", pbr->getRoughnessPtr(), 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("Metallic", pbr->getMetallicPtr(), 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("AO", pbr->getAOPtr());

            static std::string selectedAlbedoMap;
            for (const auto& texturePair : ResourceManager::Instance()->getTextureMap()) {
                if (pbr->getAlbedoMap() == texturePair.second)
                    selectedAlbedoMap = texturePair.first;
            }
            if (ImGui::BeginCombo("Albedo Map", selectedAlbedoMap.c_str())) {
                bool noMap = pbr->getAlbedoMap() == nullptr;
                if (ImGui::Selectable("--- None---", noMap)) {
                    pbr->setAlbedoMap(nullptr);
                    selectedAlbedoMap = std::string();
                }
                if (noMap) {
                    ImGui::SetItemDefaultFocus();
                }

                for (const auto& texturePair : ResourceManager::Instance()->getTextureMap()) {
                    bool isSelected = (pbr->getAlbedoMap() == texturePair.second);
                    if (ImGui::Selectable(texturePair.first.c_str(), isSelected)) {
                        pbr->setAlbedoMap(const_cast<Texture*>(texturePair.second));
                    }
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            static std::string selectedRoughnessMap;
            for (const auto& texturePair : ResourceManager::Instance()->getTextureMap()) {
                if (pbr->getRoughnessMap() == texturePair.second)
                    selectedRoughnessMap = texturePair.first;
            }
            if (ImGui::BeginCombo("Roughness Map", selectedRoughnessMap.c_str())) {
                bool noMap = pbr->getRoughnessMap() == nullptr;
                if (ImGui::Selectable("--- None---", noMap)) {
                    pbr->setRoughnessMap(nullptr);
                    selectedRoughnessMap = std::string();
                }
                if (noMap) {
                    ImGui::SetItemDefaultFocus();
                }

                for (const auto& texturePair : ResourceManager::Instance()->getTextureMap()) {
                    bool isSelected = (pbr->getRoughnessMap() == texturePair.second);
                    if (ImGui::Selectable(texturePair.first.c_str(), isSelected)) {
                        pbr->setRoughnessMap(const_cast<Texture*>(texturePair.second));
                    }
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            static std::string selectedMetallicMap;
            for (const auto& texturePair : ResourceManager::Instance()->getTextureMap()) {
                if (pbr->getMetallicMap() == texturePair.second)
                    selectedMetallicMap = texturePair.first;
            }
            if (ImGui::BeginCombo("Metallic Map", selectedMetallicMap.c_str())) {
                bool noMap = pbr->getMetallicMap() == nullptr;
                if (ImGui::Selectable("--- None---", noMap)) {
                    pbr->setMetallicMap(nullptr);
                    selectedMetallicMap = std::string();
                }
                if (noMap) {
                    ImGui::SetItemDefaultFocus();
                }

                for (const auto& texturePair : ResourceManager::Instance()->getTextureMap()) {
                    bool isSelected = (pbr->getMetallicMap() == texturePair.second);
                    if (ImGui::Selectable(texturePair.first.c_str(), isSelected)) {
                        pbr->setMetallicMap(const_cast<Texture*>(texturePair.second));
                    }
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            static std::string selectedAOMap;
            for (const auto& texturePair : ResourceManager::Instance()->getTextureMap()) {
                if (pbr->getAOMap() == texturePair.second)
                    selectedAOMap = texturePair.first;
            }
            if (ImGui::BeginCombo("AO Map", selectedAOMap.c_str())) {
                bool noMap = pbr->getAOMap() == nullptr;
                if (ImGui::Selectable("--- None---", noMap)) {
                    pbr->setAOMap(nullptr);
                    selectedAOMap = std::string();
                }
                if (noMap) {
                    ImGui::SetItemDefaultFocus();
                }

                for (const auto& texturePair : ResourceManager::Instance()->getTextureMap()) {
                    bool isSelected = (pbr->getAOMap() == texturePair.second);
                    if (ImGui::Selectable(texturePair.first.c_str(), isSelected)) {
                        pbr->setAOMap(const_cast<Texture*>(texturePair.second));
                    }
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

        } else {
            ImGui::ColorEdit4("Ambient color", material->getAmbientColorPtr());
            ImGui::ColorEdit4("Diffuse color", material->getDiffuseColorPtr());
            ImGui::ColorEdit4("Specular color", material->getSpecularColorPtr());
            ImGui::DragFloat("Specular factor", material->getSpecularFactorPtr());

            static std::string selectedDiffuseMap;
            for (const auto& texturePair : ResourceManager::Instance()->getTextureMap()) {
                if (material->getNormalMap() == texturePair.second)
                    selectedDiffuseMap = texturePair.first;
            }
            if (ImGui::BeginCombo("Diffuse Map", selectedDiffuseMap.c_str())) {
                bool noMap = material->getDiffuseMap() == nullptr;
                if (ImGui::Selectable("--- None---", noMap)) {
                    material->setDiffuseMap(nullptr);
                    selectedDiffuseMap = std::string();
                }
                if (noMap) {
                    ImGui::SetItemDefaultFocus();
                }

                for (const auto& texturePair : ResourceManager::Instance()->getTextureMap()) {
                    const std::string& textureName = texturePair.first;
                    const Texture* texture = texturePair.second;

                    // Pass nullptr as the second parameter to use the text in the map as the ID
                    bool isSelected = (material->getDiffuseMap() == texture);
                    if (ImGui::Selectable(textureName.c_str(), isSelected)) {
                        // Update the selected texture when an option is selected
                        material->setDiffuseMap(const_cast<Texture*>(texture));
                    }
                    if (isSelected) {
                        // Set the initial focus on the currently selected texture
                        selectedDiffuseMap = textureName;
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            static std::string selectedSpecularTexture;
            for (const auto& texturePair : ResourceManager::Instance()->getTextureMap()) {
                if (material->getSpecularMap() == texturePair.second)
                    selectedSpecularTexture = texturePair.first;
            }
            if (ImGui::BeginCombo("Specular Map", selectedSpecularTexture.c_str())) {
                bool noMap = material->getSpecularMap() == nullptr;
                if (ImGui::Selectable("--- None---", noMap)) {
                    material->setSpecularMap(nullptr);
                    selectedSpecularTexture = std::string();
                }
                if (noMap) {
                    ImGui::SetItemDefaultFocus();
                }

                for (const auto& texturePair : ResourceManager::Instance()->getTextureMap()) {
                    const std::string& textureName = texturePair.first;
                    const Texture* texture = texturePair.second;

                    // Pass nullptr as the second parameter to use the text in the map as the ID
                    bool isSelected = (material->getSpecularMap() == texture);
                    if (ImGui::Selectable(textureName.c_str(), isSelected)) {
                        // Update the selected texture when an option is selected
                        material->setSpecularMap(const_cast<Texture*>(texture));
                    }
                    if (isSelected) {
                        // Set the initial focus on the currently selected texture
                        selectedSpecularTexture = textureName;
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

        }

        static std::string selectedNormalMap;
        for (const auto& texturePair : ResourceManager::Instance()->getTextureMap()) {
            if (material->getNormalMap() == texturePair.second)
                selectedNormalMap = texturePair.first;
        }
        if (ImGui::BeginCombo("Normal Map", selectedNormalMap.c_str())) {
            bool noMap = material->getNormalMap() == nullptr;
            if (ImGui::Selectable("--- None---", noMap)) {
                material->setNormalMap(nullptr);
                selectedNormalMap = std::string();
            }
            if (noMap) {
                ImGui::SetItemDefaultFocus();
            }

            for (const auto& texturePair : ResourceManager::Instance()->getTextureMap()) {
                const std::string& textureName = texturePair.first;
                const Texture* texture = texturePair.second;

                bool isSelected = (material->getNormalMap() == texture);
                if (ImGui::Selectable(textureName.c_str(), isSelected)) {
                    material->setNormalMap(const_cast<Texture*>(texture));
                }
                if (isSelected) {
                    selectedNormalMap = textureName;
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
    }
}

#include "AssetBrowser.hpp"

#include "editor/ImGuiLayer.hpp"
#include "imgui/imgui.h"

#include "ResourceManager.hpp"
#include "SoundManager.hpp"

namespace Villain {

    void AssetBrowser::render() {
        ImGui::Begin("Asset Browser");

        editor->renderIcon("\uf5aa"); ImGui::SameLine();
        if (ImGui::TreeNode("Materials")) {
            for (auto const& t: ResourceManager::Instance()->getMaterialMap()) {
                if (ImGui::TreeNode(t.first.c_str())) {
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
        editor->renderIcon("\uf001"); ImGui::SameLine();
        if (ImGui::TreeNode("Music")) {
            for (auto const& t: SoundManager::Instance()->getMusicMap()) {
                if (ImGui::TreeNode(t.first.c_str())) {
                    SoundManager::Instance()->playMusic(t.first);

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
        editor->renderIcon("\uf001"); ImGui::SameLine();
        if (ImGui::TreeNode("Sound FX")) {
            for (auto const& t: SoundManager::Instance()->getSoundFXMap()) {
                if (ImGui::TreeNode(t.first.c_str())) {
                    SoundManager::Instance()->playSound(t.first);

                    ImGui::TreePop();
                }
            }

            ImGui::TreePop();
        }

        editor->renderIcon("\uf1fc"); ImGui::SameLine();
        if (ImGui::TreeNode("Textures")) {
            for (auto const& t: ResourceManager::Instance()->getTextureMap()) {
                if (ImGui::TreeNode(t.first.c_str())) {
                    float width = ImGui::GetContentRegionAvail().x;
                    float height = ImGui::GetContentRegionAvail().y;

                    ImGui::Image(
                            reinterpret_cast<ImTextureID>(t.second->getID()),
                            //ImGui::GetWindowSize(), // will respect aspect ratio of image
                            ImGui::GetContentRegionAvail(), // will squish image to fit it in
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
}

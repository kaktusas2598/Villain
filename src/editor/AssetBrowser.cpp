#include "AssetBrowser.hpp"

#include "imgui/imgui.h"

#include "ResourceManager.hpp"
#include "SoundManager.hpp"

namespace Villain {

    void AssetBrowser::render() {
        ImGui::Begin("Asset Browser");
        {
            if (ImGui::TreeNode("Assets")) {
                if (ImGui::TreeNode("Music")) {
                    for (auto const& t: SoundManager::Instance()->getMusicMap()) {
                        if (ImGui::TreeNode(t.first.c_str())) {
                            SoundManager::Instance()->playMusic(t.first);

                            ImGui::TreePop();
                        }
                    }

                    ImGui::TreePop();
                }
                if (ImGui::TreeNode("Shaders")) {
                    for (auto const& t: ResourceManager::Instance()->getShaderMap()) {
                        if (ImGui::TreeNode(t.first.c_str())) {
                            ImGui::TreePop();
                        }
                    }

                    ImGui::TreePop();
                }
                if (ImGui::TreeNode("Sound FX")) {
                    for (auto const& t: SoundManager::Instance()->getSoundFXMap()) {
                        if (ImGui::TreeNode(t.first.c_str())) {
                            SoundManager::Instance()->playSound(t.first);

                            ImGui::TreePop();
                        }
                    }

                    ImGui::TreePop();
                }
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
            }
        }
        ImGui::End();
    }
}

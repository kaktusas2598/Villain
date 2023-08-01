#include "FileBrowser.hpp"

#include "Application.hpp"
#include "Engine.hpp"
#include "ImGuiLayer.hpp"
#include "Logger.hpp"
#include "SceneWriter.hpp"

namespace Villain {

    bool FileBrowser::popup = false;
    bool FileBrowser::saveMode = false;

    void FileBrowser::render() {
        // Get the current working directory
        const fs::path& currentPath = fs::current_path();

        ImGui::Begin("File Browser");
        {
            // Customize the style
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));

            if (ImGui::BeginPopupModal("FileBrowserPopup", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar)) {

                ImGui::Text("File Browser"); ImGui::SameLine();
                // Add a small red "X" button in the right corner to close the popup
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - 20.0);
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.9, 0.1, 0.2, 1.0));
                if (ImGui::Button("X", ImVec2(20.0, 20.0))) {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::PopStyleColor();

                // Draw a background
                ImDrawList* drawList = ImGui::GetWindowDrawList();
                ImVec2 windowPos = ImGui::GetWindowPos();
                ImVec2 windowSize = ImGui::GetWindowSize();
                ImVec4 backgroundColor = ImVec4(0.2f, 0.2f, 0.2f, 0.8f);
                drawList->AddRectFilled(windowPos, ImVec2(windowPos.x + windowSize.x, windowPos.y + windowSize.y), ImGui::ColorConvertFloat4ToU32(backgroundColor));

                // Add borders
                ImVec2 minBound = windowPos;
                ImVec2 maxBound = ImVec2(windowPos.x + windowSize.x, windowPos.y + windowSize.y);
                ImU32 borderColor = ImGui::ColorConvertFloat4ToU32(ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
                float borderThickness = 1.0f;
                drawList->AddRect(minBound, maxBound, borderColor, 0.0f, ImDrawCornerFlags_All, borderThickness);


                // Display the parent directory as a selectable button
                if (ImGui::Button("..")) {
                    // Navigate to the parent directory
                    const fs::path parentPath = currentPath.parent_path();
                    if (fs::exists(parentPath))
                        fs::current_path(parentPath);
                }

                drawFileBrowserPath(currentPath);

                if (saveMode) {
                    static char nodeNameBuffer[100] = "untitled.xml";
                    ImGui::InputText("Save as", nodeNameBuffer, sizeof(nodeNameBuffer)); ImGui::SameLine();
                    if (ImGui::Button("Save")) {
                        saveMode = false;
                        VILLAIN_CRIT("Selected dir {}", selectedDirectory.string());
                        SceneWriter sceneWriter;
                        sceneWriter.saveSceneGraph(currentPath.string() + "/" + nodeNameBuffer,
                                editor->getEngine()->getApplication()->getRootNode());
                        ImGui::CloseCurrentPopup();
                    }
                }

                ImGui::EndPopup();
            }
            ImGui::PopStyleColor();
            ImGui::PopStyleVar();

            //if (popup || ImGui::Button("Open File Browser")) {
            if (popup) {
                ImGui::OpenPopup("FileBrowserPopup");
                popup = false;
            }
            // Display the parent directory as a selectable button
            if (ImGui::Button("..")) {
                // Navigate to the parent directory
                const fs::path parentPath = currentPath.parent_path();
                if (fs::exists(parentPath))
                    fs::current_path(parentPath);
            }

            filter.clear();
            drawFileBrowserPath(currentPath);
        }
        ImGui::End();
    }


    void FileBrowser::drawFileBrowserPath(const fs::path& currentPath) {
        for (const auto& entry : fs::directory_iterator(currentPath)) {
            const auto& path = entry.path();

            // Skip hidden files
            if (entry.is_regular_file() && path.filename().string()[0] == '.')
                continue;

            if (entry.is_directory()) {
                bool expanded = ImGui::TreeNode(path.filename().string().c_str());
                // Position the icon at the rightmost side of the tree node
                ImGui::SameLine(ImGui::GetContentRegionAvail().x - ImGui::GetTextLineHeightWithSpacing());
                editor->renderIcon("\uf07c");

                if (expanded) {
                    // NOTE: this will only set selected dir once dir in clicked, but we will phase out using tree for file browsing
                    selectedDirectory = path;
                    drawFileBrowserPath(path);
                    ImGui::TreePop();
                }
            } else if (entry.is_regular_file()) {
                // Outputs '.xml', '.txt' etc
                std::string extension = path.extension().string();
                std::string filename = path.filename().string();

                if (!filter.empty() && extension != filter)
                    continue;

                if (ImGui::Selectable(filename.c_str())) {
                    // Dispath select file event
                    FileSelectedEvent fileSelectedEvent = FileSelectedEvent(path);
                    editor->getEngine()->getEventDispatcher()->dispatchEvent(fileSelectedEvent);
                }

                // Position the icon at the rightmost side of the selectable item
                ImGui::SameLine(ImGui::GetContentRegionAvail().x - ImGui::GetTextLineHeightWithSpacing());
                editor->renderIcon("\uf15b");
            }
        }
    }

}

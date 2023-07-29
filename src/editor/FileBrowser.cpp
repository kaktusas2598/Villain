#include "FileBrowser.hpp"

#include "Application.hpp"
#include "Engine.hpp"
#include "ImGuiLayer.hpp"
#include "Logger.hpp"

namespace Villain {

    bool FileBrowser::popup = false;
    bool FileBrowser::saveMode = false;

    void FileBrowser::render() {
        // Get the current working directory
        const fs::path& currentPath = fs::current_path();

        ImGui::Begin("File Browser");
        {
            if (ImGui::BeginPopup("FileBrowserPopup", ImGuiWindowFlags_AlwaysAutoResize)) {
                // Customize the style
                ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 2.0f);
                ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));

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
                    char nodeNameBuffer[100] = "scene.xml";
                    ImGui::InputText("Save filename", nodeNameBuffer, sizeof(nodeNameBuffer));
                    if (ImGui::Button("Save")) {
                        saveMode = false;
                        editor->getEngine()->getApplication()->saveScene(currentPath.string() + nodeNameBuffer);
                        ImGui::CloseCurrentPopup();
                    }
                }

                ImGui::PopStyleColor();
                ImGui::PopStyleVar();
                ImGui::EndPopup();

            }

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

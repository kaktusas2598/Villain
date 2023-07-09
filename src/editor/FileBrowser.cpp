#include "FileBrowser.hpp"

#include "ImGuiLayer.hpp"

namespace Villain {

    bool FileBrowser::popup = false;

    void FileBrowser::render() {
        // Get the current working directory
        const fs::path& currentPath = fs::current_path();

        ImGui::Begin("File Browser");
        {
            if (ImGui::BeginPopup("FileBrowserPopup", ImGuiWindowFlags_AlwaysAutoResize)) {
                // Display the parent directory as a selectable button
                if (ImGui::Button("..")) {
                    // Navigate to the parent directory
                    const fs::path parentPath = currentPath.parent_path();
                    if (fs::exists(parentPath))
                        fs::current_path(parentPath);
                }

                // Temporary set xml filter for popup which will be used to load scene XMLs
                filter = ".xml";
                drawFileBrowserPath(currentPath);

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
                    // Select file
                    selectedFile = path.string();
                }

                // Position the icon at the rightmost side of the selectable item
                ImGui::SameLine(ImGui::GetContentRegionAvail().x - ImGui::GetTextLineHeightWithSpacing());
                editor->renderIcon("\uf15b");
            }
        }
    }

}

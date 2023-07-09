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
                ImGui::SameLine(); editor->renderIcon("\uf07c");

                if (expanded)
                {
                    drawFileBrowserPath(path);
                    ImGui::TreePop();
                }
            }
            else if (entry.is_regular_file())
            {
                ImGui::Text("%s", path.filename().string().c_str());
                ImGui::SameLine(); editor->renderIcon("\uf15b");
            }
        }
    }

}

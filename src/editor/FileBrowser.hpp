#pragma once

#include <imgui/imgui.h>
#include <filesystem>

namespace fs = std::filesystem;

namespace Villain {

    class ImGuiLayer;

    // File browser ImGui widget for engine editor, rendered as a window or as a popup
    class FileBrowser {
        public:
            FileBrowser(ImGuiLayer* editor): editor(editor) {}
            void render();
            void openPopup(const std::string& ext={}) { filter = ext; popup = true; }

        private:
            void drawFileBrowserPath(const fs::path& currentPath);

            static bool popup; // Toggles file browser as a popup
            ImGuiLayer* editor = nullptr;
            std::string selectedFile{};
            std::string filter{};
    };
}

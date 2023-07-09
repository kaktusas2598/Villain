#pragma once

#include <imgui/imgui.h>
#include <filesystem>

namespace fs = std::filesystem;

namespace Villain {

    // File browser ImGui widget for engine editor, rendered as a window or as a popup
    class FileBrowser {
        public:
            void render();
            void openPopup() { popup = true; }

        private:
            void drawFileBrowserPath(const fs::path& currentPath);

            static bool popup; // Toggles file browser as a popup
    };
}

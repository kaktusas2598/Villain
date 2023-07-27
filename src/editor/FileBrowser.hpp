#pragma once

#include "EventDispatcher.hpp"
#include <imgui/imgui.h>
#include <filesystem>

namespace fs = std::filesystem;

namespace Villain {

    class ImGuiLayer;

    /// Dispatched upon pressed or released key
    class FileSelectedEvent : public Event {
        public:
            FileSelectedEvent(const std::string& file, const std::string& ext)
                : fileName(file), extension(ext) {}

            std::string getFileName() const { return fileName; }
            std::string getExtension() const { return extension; }

            VILLAIN_EVENT_TYPE(FileSelectedEvent);
        private:
            std::string fileName;
            std::string extension;
    };

    /// File browser ImGui widget for engine editor, rendered as a window or as a popup
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

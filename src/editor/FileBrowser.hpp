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

            FileSelectedEvent(const fs::path& fsPath): path(fsPath) {}

            std::string getFileName() const { return path.filename().string(); }
            std::string getExtension() const { return path.extension().string(); }
            std::string getParentPath() const { return path.parent_path().string(); }
            std::string getRelativePath() const { return path.relative_path().string(); }
            std::string getRootPath() const { return path.root_path().string(); }

            VILLAIN_EVENT_TYPE(FileSelectedEvent);
        private:
            fs::path path;
    };

    /// File browser ImGui widget for engine editor, rendered as a window or as a popup
    class FileBrowser {
        public:
            FileBrowser(ImGuiLayer* editor): editor(editor) {}
            void render();
            void openPopup(const std::string& ext={}) { filter = ext; popup = true; }
            void savePopup(const std::string& ext={}) { saveMode = true; openPopup(ext); }

        private:
            void drawFileBrowserPath(const fs::path& currentPath);

            static bool popup; ///< Toggles file browser as a popup
            static bool saveMode; ///< Toggle input box for save file dialog popup
            ImGuiLayer* editor = nullptr;
            std::string selectedFile{};
            fs::path selectedDirectory;
            std::string filter{};
    };
}

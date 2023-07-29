#pragma once

namespace Villain {

    class ImGuiLayer;

    // Display and preview all loaded Engine's assets
    class AssetBrowser {
        public:
            AssetBrowser(ImGuiLayer* editor): editor(editor) {}

            void render();
        private:
            ImGuiLayer* editor = nullptr;
    };
}

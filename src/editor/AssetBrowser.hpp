#pragma once

namespace Villain {

    class ImGuiLayer;
    class Material;

    // Display and preview all loaded Engine's assets
    class AssetBrowser {
        public:
            AssetBrowser(ImGuiLayer* editor): editor(editor) {}

            void render();
            void renderMaterial(Material* material);
        private:

            ImGuiLayer* editor = nullptr;
    };
}

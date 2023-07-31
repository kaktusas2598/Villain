#pragma once

struct nk_context;

namespace Villain {

    /// Abstracts nuklear ui calls to create windows and UI elements
    class UIManager {
        public:
            UIManager();
            void init(struct nk_context* nkContext);
            void render();

            bool beginWindow(const char* title, float x, float y, int width, int height);
            void endWindow();

            bool Button(const char* label);

        private:
            struct nk_context* nuklearContext;
    };
}

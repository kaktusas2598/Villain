#include "UIManager.hpp"

#include "nuklear.h"

namespace Villain {

    UIManager::UIManager() : nuklearContext(nullptr) {}

    void UIManager::init(struct nk_context* nkContext) {
        nuklearContext = nkContext;
    }

    void UIManager::render() {
        // Implement any additional rendering logic if needed
    }

    ///////////////////////////////////////////////////////////////////////////
    // Window begin/end defines when to start rendering to new UI window and when to end
    ///////////////////////////////////////////////////////////////////////////

    bool UIManager::beginWindow(const char* title, float x, float y, int width, int height) {
        return nk_begin(nuklearContext, title, nk_rect(x, y, width, height), NK_WINDOW_BORDER);
    }

    void UIManager::endWindow() {
        nk_end(nuklearContext);
    }

    ///////////////////////////////////////////////////////////////////////////
    // UI Elements
    ///////////////////////////////////////////////////////////////////////////

    bool UIManager::Button(const char* label) {
        return nk_button_label(nuklearContext, label);
    }

}

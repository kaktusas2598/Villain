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

    bool UIManager::beginWindow(const char* title, float x, float y, int width, int height) {
        return nk_begin(nuklearContext, title, nk_rect(x, y, width, height), NK_WINDOW_BORDER);
    }

    void UIManager::endWindow() {
        nk_end(nuklearContext);
    }

    void UIManager::layoutRowDynamic(float height, int cols) {
        nk_layout_row_dynamic(nuklearContext, height, cols);
    }

    bool UIManager::button(const char* label) {
        return nk_button_label(nuklearContext, label);
    }
}

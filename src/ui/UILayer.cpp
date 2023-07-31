#include "UILayer.hpp"

#include "Window.hpp"

#include "nuklear.h"
#include "nuklear_sdl_gl3.h"

// NOTE: temporary nuklear defines
#define MAX_VERTEX_MEMORY 512 * 1024
#define MAX_ELEMENT_MEMORY 128 * 1024

namespace Villain {

    UILayer::UILayer(Engine* e): engine(e) {}

    void UILayer::init(Window& window) {
        nuklearContext = nk_sdl_init(window.getSDLWindow());
        manager.init(nuklearContext);
        /* Load Fonts: if none of these are loaded a default font will be used  */
        /* Load Cursor: if you uncomment cursor loading please hide the cursor */
        {
            struct nk_font_atlas *atlas;
            nk_sdl_font_stash_begin(&atlas);
            //struct nk_font *roboto = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Roboto-Regular.ttf", 16, 0);
            nk_sdl_font_stash_end();
            //nk_style_load_all_cursors(nuklearContext, atlas->cursors);
            //nk_style_set_font(nuklearContext, &roboto->handle);
        }
    }

    void UILayer::beginInput() {
        nk_input_begin(nuklearContext);
    }

    void UILayer::processInput(SDL_Event* event) {
        nk_sdl_handle_event(event);
    }

    void UILayer::endInput() {
        nk_input_end(nuklearContext);
    }

    void UILayer::render() {
        /* IMPORTANT: `nk_sdl_render` modifies some global OpenGL state
         * with blending, scissor, face culling, depth test and viewport and
         * defaults everything back into a default state.
         * Make sure to either a.) save and restore or b.) reset your own state after
         * rendering the UI. */
        nk_sdl_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_MEMORY, MAX_ELEMENT_MEMORY);

    }

    void UILayer::exit() {
        nk_sdl_shutdown();
    }

}

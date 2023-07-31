#pragma once

#include <SDL2/SDL_events.h>

struct nk_context;

namespace Villain {

    class Engine;
    class Window;

    /// Nukear UI abstraction layer
    class UILayer {
        public:
            UILayer(Engine* e);

            /// Sets up Nuklear UI context and font atlas
            void init(Window& window);

            /// Call only before processing inputs
            void beginInput();
            /// Forward SDL events to nuklear
            void processInput(SDL_Event* event);
            /// Call after processing inputs
            void endInput();

            /// Render main application UI
            void render();

            /// Nuklear ui shutdown
            void exit();

            struct nk_context * getNuklearContext() { return nuklearContext; }
            void setNulkearContext(struct nk_context* nctx) { nuklearContext = nctx; }

        private:
            Engine* engine = nullptr;
            struct nk_context* nuklearContext; ///< Nuklear UI context
    };
}

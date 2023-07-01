#ifndef __KEY_CODE__
#define __KEY_CODE__

#include "SDL_keycode.h"

namespace Villain {

    enum class KeyCode {
        w = SDLK_w,
        a = SDLK_a,
        s = SDLK_s,
        d = SDLK_d,
        SPACE = SDLK_SPACE,
        LSHIFT = SDLK_LSHIFT
    };
}

#endif // !__KEY_CODE__

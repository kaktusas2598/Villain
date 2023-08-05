#pragma once

#include <SDL2/SDL_keycode.h>

namespace Villain {

    enum class KeyCode {

        UKNOWN = SDLK_UNKNOWN,

        RETURN = SDLK_RETURN,
        ESCAPE = SDLK_ESCAPE,
        BACKSPACE = SDLK_BACKSPACE,
        TAB = SDLK_TAB,
        SPACE = SDLK_SPACE,
        EXCLAIM = SDLK_EXCLAIM,
        QUOTEBL = SDLK_QUOTEDBL,
        HACK = SDLK_HASH,
        PERCENT = SDLK_PERCENT,
        DOLLAR = SDLK_DOLLAR,
        AMPERSAND = SDLK_AMPERSAND,
        QUOTE = SDLK_QUOTE,
        LEFTPAREN = SDLK_LEFTPAREN,
        RIGHTPAREN = SDLK_RIGHTPAREN,
        ASTERISK = SDLK_ASTERISK,
        PLUS = SDLK_PLUS,
        COMMA = SDLK_COMMA,
        MINUS = SDLK_MINUS,
        PERIOD = SDLK_PERIOD,
        SLASH = SDLK_SLASH,
        NUM0 = SDLK_0,
        NUM1 = SDLK_1,
        NUM2 = SDLK_2,
        NUM3 = SDLK_3,
        NUM4 = SDLK_4,
        NUM5 = SDLK_5,
        NUM6 = SDLK_6,
        NUM7 = SDLK_7,
        NUM8 = SDLK_8,
        NUM9 = SDLK_9,
        COLON = SDLK_COLON,
        SEMICOLON = SDLK_SEMICOLON,
        LESS = SDLK_LESS,
        EQUALS = SDLK_EQUALS,
        GREATER = SDLK_GREATER,
        QUESTION = SDLK_QUESTION,
        AT = SDLK_AT,

        // TODO: add rest of letters, modifiers, function keys and mouse keys
        w = SDLK_w,
        a = SDLK_a,
        s = SDLK_s,
        d = SDLK_d,

        r = SDLK_r,
        LSHIFT = SDLK_LSHIFT
    };
}

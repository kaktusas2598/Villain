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

        a = SDLK_a,
        b = SDLK_b,
        c = SDLK_c,
        d = SDLK_d,
        e = SDLK_e,
        f = SDLK_f,
        g = SDLK_g,
        h = SDLK_h,
        i = SDLK_i,
        j = SDLK_j,
        k = SDLK_k,
        l = SDLK_l,
        m = SDLK_m,
        n = SDLK_n,
        o = SDLK_o,
        p = SDLK_p,
        q = SDLK_q,
        r = SDLK_r,
        s = SDLK_s,
        t = SDLK_t,
        u = SDLK_u,
        v = SDLK_v,
        w = SDLK_w,
        x = SDLK_x,
        y = SDLK_y,
        z = SDLK_z,

        // TODO: add rest of modifiers, function keys and mouse keys, and etc
        LSHIFT = SDLK_LSHIFT
    };
}

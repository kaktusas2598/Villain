#include "InputManager.hpp"

#include "imgui/imgui.h"
#include <iostream>

namespace Villain {

    InputManager* InputManager::s_pInstance = 0;

    InputManager::InputManager(): mouseCoords(0.0f, 0.0f) { }

    void InputManager::update() {
        //iterate through current key map and copy it to the previous keymap
        for (auto& it : keyMap)
        {
            prevKeyMap[it.first] = it.second;
        }

        //reset mouseWheel to 0
        mousewheel = 0;
    }

    void InputManager::pressKey(unsigned int keyID) {
        keyMap[keyID] = true;
    }

    void InputManager::releaseKey(unsigned int keyID) {
        keyMap[keyID] = false;
    }

    void InputManager::setMouseCoords(float x, float y) {
        mouseCoords.x = x;
        mouseCoords.y = y;
    }

    bool InputManager::isKeyDown(unsigned int keyID) {
        //search for key
        auto it = keyMap.find(keyID);
        if (it != keyMap.end())
        {
            //return the found key
            return it->second;
        }
        else { return false; }
    }

    bool InputManager::isKeyPressed(unsigned int keyID) {
        //check if key was pressed on the current frame and
        //not on the previous frame
        if (isKeyDown(keyID) == true && wasKeyDown(keyID) == false) {
            return true;
        }

        return false;
    }

    bool InputManager::wasKeyDown(unsigned int keyID) {
        //search for key in previous key map
        auto it = prevKeyMap.find(keyID);
        if (it != prevKeyMap.end()) {
            //return the found key
            return it->second;
        }
        else { return false; }
    }

    void InputManager::setMouseWheel(int m) {
        if (m == 1)
        {
            mousewheel = 1;
        }
        else if (m == -1)
        {
            mousewheel = -1;
        }
        else
        {
            mousewheel = 0;
        }
    }

    int ImTextCharFromUtf8(unsigned int* out_char, const char* in_text, const char* in_text_end) {
        unsigned int c = (unsigned int)-1;
        const unsigned char* str = (const unsigned char*)in_text;
        if (!(*str & 0x80))
        {
            c = (unsigned int)(*str++);
            *out_char = c;
            return 1;
        }
        if ((*str & 0xe0) == 0xc0)
        {
            *out_char = 0xFFFD; // will be invalid but not end of string
            if (in_text_end && in_text_end - (const char*)str < 2) return 1;
            if (*str < 0xc2) return 2;
            c = (unsigned int)((*str++ & 0x1f) << 6);
            if ((*str & 0xc0) != 0x80) return 2;
            c += (*str++ & 0x3f);
            *out_char = c;
            return 2;
        }
        if ((*str & 0xf0) == 0xe0)
        {
            *out_char = 0xFFFD; // will be invalid but not end of string
            if (in_text_end && in_text_end - (const char*)str < 3) return 1;
            if (*str == 0xe0 && (str[1] < 0xa0 || str[1] > 0xbf)) return 3;
            if (*str == 0xed && str[1] > 0x9f) return 3; // str[1] < 0x80 is checked below
            c = (unsigned int)((*str++ & 0x0f) << 12);
            if ((*str & 0xc0) != 0x80) return 3;
            c += (unsigned int)((*str++ & 0x3f) << 6);
            if ((*str & 0xc0) != 0x80) return 3;
            c += (*str++ & 0x3f);
            *out_char = c;
            return 3;
        }
        if ((*str & 0xf8) == 0xf0)
        {
            *out_char = 0xFFFD; // will be invalid but not end of string
            if (in_text_end && in_text_end - (const char*)str < 4) return 1;
            if (*str > 0xf4) return 4;
            if (*str == 0xf0 && (str[1] < 0x90 || str[1] > 0xbf)) return 4;
            if (*str == 0xf4 && str[1] > 0x8f) return 4; // str[1] < 0x80 is checked below
            c = (unsigned int)((*str++ & 0x07) << 18);
            if ((*str & 0xc0) != 0x80) return 4;
            c += (unsigned int)((*str++ & 0x3f) << 12);
            if ((*str & 0xc0) != 0x80) return 4;
            c += (unsigned int)((*str++ & 0x3f) << 6);
            if ((*str & 0xc0) != 0x80) return 4;
            c += (*str++ & 0x3f);
            // utf-8 encodings of values used in surrogate pairs are invalid
            if ((c & 0xFFFFF800) == 0xD800) return 4;
            *out_char = c;
            return 4;
        }
        *out_char = 0;
        return 0;
    }

    int ImTextStrFromUtf8(ImWchar* buf, int buf_size, const char* in_text, const char* in_text_end, const char** in_text_remaining) {
        ImWchar* buf_out = buf;
        ImWchar* buf_end = buf + buf_size;
        while (buf_out < buf_end-1 && (!in_text_end || in_text < in_text_end) && *in_text)
        {
            unsigned int c;
            in_text += ImTextCharFromUtf8(&c, in_text, in_text_end);
            if (c == 0)
                break;
            if (c < 0x10000)    // FIXME: Losing characters that don't fit in 2 bytes
                *buf_out++ = (ImWchar)c;
        }
        *buf_out = 0;
        if (in_text_remaining)
            *in_text_remaining = in_text;
        return (int)(buf_out - buf);
    }

    void InputManager::addInputCharacter(char c) {
        //get string length
        int x = 0;
        while(inChars[x++]);
        const int n = x;

        //if we're not at the end of the string, add character to inchars
        if (n < sizeof(inChars)/sizeof(char))
        {
            inChars[n] = c;
            inChars[n+1] = '\0';
        }
    }

    void InputManager::addInputCharacters(const char* chars) {
        for (int i = 0; i < sizeof(chars)/sizeof(const char*); i++)
        {
            inChars[i] = chars[i];
        }
    }
}

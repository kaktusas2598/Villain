#pragma once

#include <SDL2/SDL.h>
#include <stdio.h>
#include <unordered_map>
#include <vector>
#include <iostream>
#include "glm/glm.hpp"

namespace Villain {

    enum mouseButtons {
        LEFT = 0,
        MIDDLE = 1,
        RIGHT = 2
    };

    /*! \brief Keyboard and Mouse input manager.
     *
     *  Manages unordered map currently and previously pressed keys and also holds mouse coordinates.
     *  Can be used to set pressed keys and mouse coords, and also to query for presse keys or coordinates.
     *  @sa Engine::handleEvents, EventDispatcher
     */
    class Input {
        public:
            static Input* Get() {
                if (s_pInstance == 0) {
                    s_pInstance = new Input();
                }
                return s_pInstance;
            }

            /// Copies current key map to previous one, gets called in Main Engine loop
            void update();

            /// Set the key value in map to true(pressed)
            void pressKey(unsigned int keyID);
            /// Set the key value in map to false(released)
            void releaseKey(unsigned int keyID);

            /// Helper func for updating the current event type
            void setEventType(Uint32 type) { eventType = type; }

            /// Tells the input manager the location of the cursor
            void setMouseCoords(float x, float y);

            /// Set relative mouse motion, used by 3D Camera
            void setMouseOffsets(float x, float y);

            /// Returns true if key is held down
            bool isKeyDown(unsigned int keyID);

            /// Returns true if key is pressed
            bool isKeyPressed(unsigned int keyID);

            /// Updates the mouse wheel
            void setMouseWheel(int m);
            int mousewheel = 0; ///< Mouse wheel's value

            /// Add characters into inChars
            void addInputCharacter(char c);
            /// Add characters into inChars from a string
            void addInputCharacters(const char* chars);

            glm::vec2 getMouseCoords() const { return mouseCoords; } ///<Mouse coords's geeter
            glm::vec2 getMouseOffsets() const { return mouseOffsets; }
            bool getMouseButtonState(int buttonNumber) {return mouseButtonStates[buttonNumber]; }

            Uint32 getEventType() { return eventType; } //< SDL_Event type's getter
            const char* getInChars() { return inChars; }

        private:
            Input();
            ~Input() {};

            //singleton instance
            static Input* s_pInstance;

            /// Identical to isKeyDown but searches the previous key map
            bool wasKeyDown(unsigned int keyID);

            std::unordered_map<unsigned int, bool> keyMap; ///< Holds map of SDL key values versus pressed/released bool values
            std::unordered_map<unsigned int, bool> prevKeyMap; ///< Holds map of previously pressed keys

            std::vector<bool> mouseButtonStates;
            glm::vec2 mouseCoords;
            glm::vec2 mouseOffsets; ///< Using SDL relative mouse mode

            Uint32 eventType; ///< event type: keypress, mousemotion, etc...

            char inChars[1];
    };
}

#pragma once

#include <string>

namespace Villain {

    class ScriptEditor {
        public:
            ScriptEditor() {}

            void setScript(const std::string& fileName);

            void render();
        private:
            std::string filePath;
            std::string text;
            unsigned bufferSize = 0;
            bool shouldSetFocus = false;
    };
}

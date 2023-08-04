#pragma once

#include <string>

namespace Villain {

    class AudioLoader {
        public:
            static unsigned loadWAV(const std::string& fileName);
    };
}

#pragma once

#include "AL/al.h"
#include <string>

namespace Villain {

    /// Represents audio buffer which can be played and used by multiple sources
    class AudioBuffer {
        public:
            AudioBuffer(const std::string& fileName);
            ~AudioBuffer();

            ALuint getID() const { return bufferId; }
        private:
            ALuint bufferId;
    };
}

#pragma once

#include <AL/al.h>
#include <string>

namespace Villain {

    class AudioBuffer {
        public:
            AudioBuffer(const std::string& fileName);
            ~AudioBuffer();

            // TODO: callback stuff
            void play();
            // TODO: other playback controls
            // Volume control
            // Setting 3D sound position!
        private:
            ALuint bufferId;
            ALuint sourceId;
    };
}

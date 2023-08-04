#pragma once

#include "AL/al.h"
#include "AL/alc.h"
#include <string>

namespace Villain {

    class AudioEngine {
        public:
            AudioEngine();
            ~AudioEngine();

            // TODO: temporary - this is blocking call! But it's possible to set callback
            void playWAV(ALuint audioBuffer);
        private:
            ALCdevice* device = nullptr;
            ALCcontext* context = nullptr;
    };
}

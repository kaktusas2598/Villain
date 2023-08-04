#pragma once

#include "AL/al.h"
#include "AL/alc.h"
#include <string>

namespace Villain {

    class AudioEngine {
        public:
            AudioEngine();
            ~AudioEngine();

            /// Set 3D position to create realistic 3D audio environment
            void setListenerPosition(ALfloat x, ALfloat y, ALfloat z);
            /// Set 3D orientation to create realistic 3D audio environment
            void setListenerOrientation(ALfloat forwardX, ALfloat forwardY, ALfloat forwardZ, ALfloat upX, ALfloat upY, ALfloat upZ);
        private:
            ALCdevice* device = nullptr;
            ALCcontext* context = nullptr;
    };
}

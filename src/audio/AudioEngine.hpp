#pragma once

#include "AL/al.h"
#include "AL/alc.h"
#include "glm/glm.hpp"
#include <string>

namespace Villain {

    /// Manages OpenAL context and updates listener
    class AudioEngine {
        public:
            AudioEngine();
            ~AudioEngine();

            void update(const glm::vec3& listenerPos, const glm::vec3& listenerForward, const glm::vec3& listenerUp);

            /// Set 3D position to create realistic 3D audio environment
            void setListenerPosition(const glm::vec3& pos);
            /// Set 3D orientation to create realistic 3D audio environment
            void setListenerOrientation(const glm::vec3& forward, const glm::vec3& up);
        private:
            ALCdevice* device = nullptr;
            ALCcontext* context = nullptr;
    };
}

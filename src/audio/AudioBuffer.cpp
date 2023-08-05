#include "AudioBuffer.hpp"

#include "AudioLoader.hpp"
#include "Logger.hpp"

namespace Villain {

    AudioBuffer::AudioBuffer(const std::string& fileName) {
        VILLAIN_INFO("Loading audio: {} ", fileName);
        bufferId = AudioLoader::loadAudio(fileName);
    }

    AudioBuffer::~AudioBuffer() {
        alDeleteBuffers(1, &bufferId);
    }
}

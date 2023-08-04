#include "AudioLoader.hpp"

#include "Logger.hpp"

#include <AL/al.h>
#include <sndfile.h>

namespace Villain {

    unsigned AudioLoader::loadWAV(const std::string& fileName) {
        SF_INFO fileInfo;
        // Load audio data using libsndfile
        SNDFILE* sndFile = sf_open(fileName.c_str(), SFM_READ, &fileInfo);

        if (!sndFile) {
            VILLAIN_CRIT("Failed loading audio file!");
            // Error handling
        }

        // Create OpenAL buffer to hold audio data
        ALuint buffer;
        alGenBuffers(1, &buffer);

        // Read audio data from libsndfile and populate the OpenAL buffer
        const sf_count_t numFrames = fileInfo.frames;
        const sf_count_t numChannels = fileInfo.channels;
        const sf_count_t numSamples = numFrames * numChannels;
        float* samples = new float[numSamples];

        sf_readf_float(sndFile, samples, numFrames);

        VILLAIN_WARN("Sample rate: {}", fileInfo.samplerate);
        VILLAIN_WARN("Num channels: {}", numChannels);
        VILLAIN_WARN("Bit depth: {}", fileInfo.format);

        // Convert floating-point samples to 16-bit signed integer samples
        ALshort* bufferData = new ALshort[numSamples];
        for (int i = 0; i < numSamples; i++) {
            // Scale the floating-point sample to the range of a 16-bit signed integer (-32768 to 32767)
            bufferData[i] = static_cast<ALshort>(samples[i] * 32767.0f);
        }

        alBufferData(buffer, (numChannels == 2) ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16,
                bufferData, numSamples * sizeof(ALshort), fileInfo.samplerate);

        delete[] samples;
        sf_close(sndFile);

        return buffer;
    }
}

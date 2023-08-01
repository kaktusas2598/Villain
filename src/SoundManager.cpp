#include "SoundManager.hpp"
#include "ErrorHandler.hpp"

namespace Villain {

    SoundManager* SoundManager::sInstance = 0;

    SoundManager::SoundManager() {
        if (Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG) == -1) {
            exitWithError("Mix Init Error: " + std::string(Mix_GetError()));
        }

        // (int frequency, Uint16 format, int channels, int chunksize)
        if (Mix_OpenAudio(22050, AUDIO_S16, 2, 4096) == -1) {
            exitWithError("Mix OpenAudio Error: " + std::string(Mix_GetError()));
        }
    }

    SoundManager::~SoundManager() {
        Mix_CloseAudio();
    }

    bool SoundManager::load(std::string fileName, std::string id, SoundType type, int volumeAdjust) {
        if (type == SOUND_MUSIC) {
            if (musicMap.find(id) != musicMap.end()) {
                return true;
            }

            Mix_Music* music = Mix_LoadMUS(fileName.c_str());
            if (music == 0) {
                exitWithError("Could not load music file: " + std::string(Mix_GetError()));
                return false;
            }

            if (volumeAdjust != 0) {
                int currVolume = Mix_VolumeMusic(-1);
                int newVolume = currVolume + volumeAdjust;
                if (volumeAdjust > MIX_MAX_VOLUME) {
                    newVolume = MIX_MAX_VOLUME;
                } else if (newVolume < 0) {
                    newVolume = 0;
                }
                Mix_VolumeMusic(newVolume);
            }

            musicMap[id] = music;
            return true;
        } else if (type == SOUND_SFX) {
            if (sFXMap.find(id) != sFXMap.end()) {
                return true;
            }

            Mix_Chunk* fx = Mix_LoadWAV(fileName.c_str());
            if (fx == 0) {
                exitWithError("Could not load wav file: " + std::string(Mix_GetError()));
                return false;
            }

            if (volumeAdjust != 0) {
                int currVolume = Mix_VolumeChunk(fx, -1);
                int newVolume = currVolume + volumeAdjust;
                if (volumeAdjust > MIX_MAX_VOLUME) {
                    newVolume = MIX_MAX_VOLUME;
                } else if (newVolume < 0) {
                    newVolume = 0;
                }
                Mix_VolumeChunk(fx, newVolume);
            }

            sFXMap[id] = fx;
            return true;
        }
        return false;
    }

    void SoundManager::playSound(std::string id, int loop) {
        // -1 means any channel
        if (Mix_PlayChannel(-1, sFXMap[id], loop) == -1) {
            // Failed to play sound effect on any channel
        }
    }

    void SoundManager::playMusic(std::string id, int loop) {
        Mix_PlayMusic(musicMap[id], loop);
    }

    void SoundManager::pauseMusic() {
        Mix_PauseMusic();
    }

    void SoundManager::stopMusic() {
        Mix_HaltMusic();
    }

    void SoundManager::resumeMusic() {
        Mix_ResumeMusic();
    }

    void SoundManager::cleanSoundMaps() {
        for (const auto& soundPair: sFXMap) {
            Mix_FreeChunk(sFXMap[soundPair.first]);
        }
        sFXMap.clear();
        for (const auto& soundPair: musicMap) {
            Mix_FreeMusic(musicMap[soundPair.first]);
        }
        musicMap.clear();
    }
}

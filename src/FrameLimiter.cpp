#include "FrameLimiter.hpp"

#include <SDL2/SDL.h>

namespace Villain {

    void FrameLimiter::init(float maxFPS) {
        setMaxFPS(maxFPS);
    }

    void FrameLimiter::setMaxFPS(float maxFPS) {
        m_maxFPS = maxFPS;
    }

    void FrameLimiter::begin() {
        m_startTicks = SDL_GetTicks();
    }

    float FrameLimiter::end() {
        calculateFPS();

        //time the frame took
        float frameTicks = SDL_GetTicks() - m_startTicks;

        //limit the FPS to the max FPS
        //so if the frame took less time than the max fps,
        //delay for the difference
        if (1000.0f / m_maxFPS > frameTicks)
        {
            SDL_Delay(1000.0f / m_maxFPS - frameTicks);
        }

        return m_fps;
    }

    void FrameLimiter::calculateFPS() {

        //taking the average over 10 frames
        static const int NUM_SAMPLES = 10;
        //frame times buffer
        static float frameTimes[NUM_SAMPLES];
        //the current frame the game is on
        static int currentFrame = 0;
        //the ticks on the previous frame
        static float prevTicks = SDL_GetTicks();
        //the ticks on the current frame
        float currentTicks = SDL_GetTicks();
        //the average frame time throughout the program
        float frameTimeAverage = 0;

        m_frameTime = currentTicks - prevTicks;
        frameTimes[currentFrame % NUM_SAMPLES] = m_frameTime;

        //update previous ticks
        prevTicks = currentTicks;

        int count;

        currentFrame++;

        currentFrame < NUM_SAMPLES ? count = currentFrame : count = NUM_SAMPLES;
//        if (currentFrame < NUM_SAMPLES)
//        {
//            count = currentFrame;
//        }
//        else
//        {
//            count = NUM_SAMPLES;
//        }

        for(int i = 0; i < count; i++)
        {
            frameTimeAverage += frameTimes[i];
        }

        frameTimeAverage /= count;

        //don't want to divide by 0
        frameTimeAverage > 0 ? m_fps = 1000.0f / frameTimeAverage : m_fps = 60.0f;
    }
}

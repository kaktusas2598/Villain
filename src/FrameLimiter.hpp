#pragma once

#include <stdio.h>

namespace Villain {

    class FrameLimiter {
    public:
        FrameLimiter() {}

        void init(float maxFPS);

        /// Sets the max FPS
        void setMaxFPS(float maxFPS);

        /// Sets the starting ticks
        void begin();

        /// Adds delay if necessary, returns the current FPS
        float end();

    private:
        /// Calculates the current FPS
        void calculateFPS();

        float m_fps;
        float m_maxFPS;
        float m_frameTime;
        unsigned int m_startTicks;

    };
}

#include "Timer.hpp"

namespace Villain {

	Timer::Timer() {
		start();
	}

	void Timer::start() {
		startTicks = SDL_GetTicks();
	}
	Uint32 Timer::read() {
		return SDL_GetTicks() - startTicks;
	}
	float Timer::readSeconds() {
		return (float)(SDL_GetTicks() - startTicks)/1000.0f;
	}
}

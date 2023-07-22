#pragma once

#include <SDL2/SDL.h>

namespace Villain {

    class Timer {
		public:
			Timer();

			void start();
			// Return elapsed time in mili seconds
			Uint32 read();
			// Return elapsed time in seconds
			float readSeconds();
		private:
			Uint32 startTicks;
    };
}

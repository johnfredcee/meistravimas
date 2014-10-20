#include <SDL.h>
#include "timer.h"

namespace venk
{

double timeNow(void)
{
	return SDL_GetPerformanceCounter() / (double) SDL_GetPerformanceFrequency();
}

}

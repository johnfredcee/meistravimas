
#include <SDL.h>
#include "tinymt32.h"
#include "random.h"

namespace venk
{

RandomContext::RandomContext()
{
	::tinymt32_init(&this->state, SDL_GetTicks());
}

RandomContext::RandomContext(Uint32 seed)
{
	::tinymt32_init(&this->state, seed);
}

float RandomContext::nextRandom()
{
	::tinymt32_generate_float(&this->state);
}

}

#ifndef RANDOM_H_INCLUDED
#define RANDOM_H_INCLUDED

namespace venk
{

	class RandomContext
	{
		::tinymt32_t state;

	public:
		// init using timer
		RandomContext();

		// init using explicit seed
		RandomContext(Uint32 seed);

		// random number between 0.0 and 1.0
		float nextRandom();

	};

};

#endif

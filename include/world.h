#ifndef WORLD_H_INCLUDED
#define WORLD_H_INCLUDED

namespace venk
{
	class World
	{
	private:
		RandomContext rand;

		static const int width  = 1024;
		static const int height = 768;

		float mAltitude[width][height];

		float get_altitude(Uint32 x, Uint32 y) const
		{
			return ((x >= width) || (y >= height)) ? 0.0f : mAltitude[x][y];
		}

	public:

		World(Uint32 seed);

		float sample_altitude(float x, float y) const;
	};

}

#endif

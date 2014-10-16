
#include <cmath>
#include <SDL.h>
#include "tinymt32.h"
#include "random.h"
#include "world.h"

namespace venk
{

float World::sample_altitude(float x, float y) const
{
	float x1 = std::floor(x);
	float x2 = std::ceil(x);
	float y1 = std::floor(y);
	float y2 = std::ceil(y);
	float  Q11 = get_altitude(x1, y1);
	float  Q12 = get_altitude(x1, y2);
	float  Q21 = get_altitude(x2, y1);
	float  Q22 = get_altitude(x2, y2);
	float  R1 = ((x2 - x) / (x2 - x1)) * Q11 + ((x - x1) / (x2 - x1)) * Q21;
	float  R2 = ((x2 - x) / (x2 - x1)) * Q12 + ((x - x1) / (x2 - x1)) * Q22;
	return ((y2 - y)/(y2 - y1)) * R1 + ((y - y1)/(y2 - y1)) * R2;
}

}

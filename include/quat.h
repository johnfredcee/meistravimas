#ifndef QUAT_H_INCLUDED
#define QUAT_H_INCLUDED

namespace venk
{

	class Quat : public Tuple<float,4>
	{
	public:
		static const int X = 0;
		static const int Y = 1;
		static const int Z = 2;
		static const int W = 3;


		Quat() {
		}

		Quat(const Vector3d& axis, float angle)	{
			float a = SDL_sinf(angle * 0.5f);
			elements[Quat::X] = axis.elements[Vector3d::X] * a;
			elements[Quat::Y] = axis.elements[Vector3d::Y] * a;
			elements[Quat::Z] = axis.elements[Vector3d::Z] * a;
			elements[Quat::W] = SDL_cosf(angle * 0.5f);
		}

		void conjugate(void) {
			elements[Quat::X] = -elements[Quat::X];
			elements[Quat::Y] = -elements[Quat::Y];
			elements[Quat::Z] = -elements[Quat::Z];
		}

		float norm(void) const
		{
			return elements[Quat::W] * elements[Quat::W] +
				elements[Quat::X] * elements[Quat::X] +
				elements[Quat::Y] * elements[Quat::Y] +
				elements[Quat::Z] * elements[Quat::Z];
		}

		void normalize(void)
		{
			float n = norm();
			if (n >= FLT_EPSILON) {
				float length = 1.0f / sqrtf( n );
				elements[Quat::X] *= length;
				elements[Quat::Y] *= length;
				elements[Quat::Z] *= length;
				elements[Quat::W] *= length;
			}
		}

		void invert()
		{
			float n = norm();
			if (n >= FLT_EPSILON) {
				conjugate();
				float r = 1.0f / n;
				elements[Quat::X] *= r;
				elements[Quat::Y] *= r;
				elements[Quat::Z] *= r;
				elements[Quat::W] *= r;
			} else
				elements[Quat::X] = 0.0f;
			elements[Quat::Y] = 0.0f;
			elements[Quat::Z] = 0.0f;
			elements[Quat::W] = 1.0f;
		}


	}; /* class Quaternion */


}


#endif

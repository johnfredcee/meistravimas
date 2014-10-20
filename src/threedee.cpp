
#include <SDL.h>
#include "tuple.h"
#include "threedee.h"
#include "quat.h"

namespace venk
{
	Vector3d operator*(const Vector3d& v, float scalar) {
		return Vector3d(v.elements[Vector3d::X] * scalar, 
						v.elements[Vector3d::Y] * scalar, 
						v.elements[Vector3d::Z] * scalar);
	}
	
	float dot(const Vector3d& a, const Vector3d& b)  {
		return (a.elements[Vector3d::X] * b.elements[Vector3d::X]) +
			(a.elements[Vector3d::Y] * b.elements[Vector3d::Y]) +
			(a.elements[Vector3d::Z] * b.elements[Vector3d::Z]);
    }
	
	Vector3d cross(const Vector3d& a, const Vector3d& b) {
		Vector3d result;
		result.elements[Vector3d::X] = a.elements[Vector3d::Y] * b.elements[Vector3d::Z] 
			- a.elements[Vector3d::Z] * b.elements[Vector3d::Y];
		result.elements[Vector3d::Y] = a.elements[Vector3d::Z] * b.elements[Vector3d::X] 
			- a.elements[Vector3d::X] * b.elements[Vector3d::Z];
		result.elements[Vector3d::Z] = a.elements[Vector3d::X] * b.elements[Vector3d::Y] 
			- a.elements[Vector3d::Y] * b.elements[Vector3d::X];
		return result;
	}

	Vector3d operator-(const Vector3d& a, const Vector3d& b) {
		Vector3d result(a.elements[Vector3d::X] - b.elements[Vector3d::X], 
					 a.elements[Vector3d::Y] - b.elements[Vector3d::Y],
					 a.elements[Vector3d::Z] - b.elements[Vector3d::Z]);
		return result;
	}

}


#include <SDL.h>
#include "tuple.h"
#include "threedee.h"
#include "quat.h"

namespace venk
{
	Vector3d operator*(const Vector3d& v, float scalar) {
		return Vector3d(v.x() * scalar, 
						v.y() * scalar, 
						v.z() * scalar);
	}
	
	float dot(const Vector3d& a, const Vector3d& b)  {
		return (a.x() * b.x()) +
			(a.y() * b.y()) +
			(a.z() * b.z());
    }
	
	Vector3d cross(const Vector3d& a, const Vector3d& b) {
		Vector3d result;
		result.x() = a.y() * b.z() 
			- a.z() * b.y();
		result.y() = a.z() * b.x() 
			- a.x() * b.z();
		result.z() = a.x() * b.y() 
			- a.y() * b.x();
		return result;
	}

	Vector3d operator-(const Vector3d& a, const Vector3d& b) {
		Vector3d result(a.x() - b.x(), 
					 a.y() - b.y(),
					 a.z() - b.z());
		return result;
	}

}

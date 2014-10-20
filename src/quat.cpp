
#include <SDL.h>
#include "tuple.h"
#include "threedee.h"
#include "quat.h"

namespace venk
{


Quat operator*(const Quat& lhs, const Quat& rhs)
{
    float qax = lhs.elements[Quat::X]; 
	float qay = lhs.elements[Quat::Y]; 
	float qaz = lhs.elements[Quat::Z];
	float qaw = lhs.elements[Quat::W];
    float qbx = rhs.elements[Quat::X]; 
	float qby = rhs.elements[Quat::Y]; 
	float qbz = rhs.elements[Quat::Z]; 
	float qbw = rhs.elements[Quat::W];

	Quat result;
    result.elements[0] = qax * qbw + qaw * qbx + qay * qbz - qaz * qby;
    result.elements[1] = qay * qbw + qaw * qby + qaz * qbx - qax * qbz;
    result.elements[2] = qaz * qbw + qaw * qbz + qax * qby - qay * qbx;
    result.elements[3] = qaw * qbw - qax * qbx - qay * qby - qaz * qbz;

    return result;    
}

Vector3d operator*(const Quat& lhs, const Vector3d& rhs)
{


    float x = rhs.elements[Vector3d::X]; 
	float y = rhs.elements[Vector3d::Y]; 
	float z = rhs.elements[Vector3d::Z];
    float qx = lhs.elements[Quat::X]; 
	float qy = lhs.elements[Quat::Y]; 
	float qz = lhs.elements[Quat::Z]; 
	float qw = lhs.elements[Quat::W];

	// calculate quat * vec
	float ix = qw * x + qy * z - qz * y;
	float iy = qw * y + qz * x - qx * z;
	float iz = qw * z + qx * y - qy * x;
	float iw = -qx * x - qy * y - qz * z;

    // calculate result * inverse quat
	Vector3d result;
    result.elements[0] = ix * qw + iw * -qx + iy * -qz - iz * -qy;
    result.elements[1] = iy * qw + iw * -qy + iz * -qx - ix * -qz;
    result.elements[2] = iz * qw + iw * -qz + ix * -qy - iy * -qx;

	return result;
}

}


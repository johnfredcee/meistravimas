#ifndef MATRIX44_H_INCLUDED
#define MATRIX44_H_INCLUDED

namespace venk {

class Matrix44 : public Tuple<float, 16> {
public:
	static const std::size_t tuple_size = 16;
	static const eScalarType element_type = BufferElementType<float>::element_type;

	/* construct, no initialisation */
	Matrix44() {
	}

	Matrix44(const Quat& q, const Vector3d& v) {
		float x = q.elements[Quat::X];
		float y = q.elements[Quat::Y];
		float z = q.elements[Quat::Z];
		float w = q.elements[Quat::W];
		float x2 = x + x;
		float y2 = y + y;
		float z2 = z + z;
		float xx = x * x2;
		float xy = x * y2;
		float xz = x * z2;
		float yy = y * y2;
		float yz = y * z2;
		float zz = z * z2;
		float wx = w * x2;
		float wy = w * y2;
		float wz = w * z2;
		elements[0] = 1.0f - (yy + zz);
		elements[1] = xy + wz;
		elements[2] = xz - wy;
		elements[3] = 0.0f;
		elements[4] = xy - wz;
		elements[5] = 1.0f - (xx + zz);
		elements[6] = yz + wx;
		elements[7] = 0.0f;
		elements[8] = xz + wy;
		elements[9] = yz - wx;
		elements[10] = 1.0f - (xx + yy);
		elements[11] = 0.0f;
		elements[12] = v.x();
		elements[13] = v.y();
		elements[14] = v.z();
		elements[15] = 1.0f;
	}

	void identity() {
		elements[1] = elements[2] = elements[3] = elements[4] = 0.0f;
		elements[6] = elements[7] = elements[8] = elements[9] = 0.0f;
		elements[11] = elements[12] = elements[13] = elements[14] = 0.0f;
		elements[0] = elements[5] = elements[10] = elements[15] = 1.0f;
	}


	void transpose() {
		for(int i = 0; i < 16; i++) {
			float tmp = elements[i];
			elements[i] = elements[ i / 4 + (i % 4) * 4 ];
			elements[ i / 4 + (i % 4) * 4 ] = tmp;
		}
	}


	void translation(const Vector3d& translation) {
		elements[1] = elements[2] = elements[3] = elements[4] = 0.0f;
		elements[6] = elements[7] = elements[8] = elements[9] = 0.0f;
		elements[11] = 0.0f;
		elements[0] = elements[5] = elements[10] = elements[15] = 1.0f;
		elements[12] = translation.x();
		elements[13] = translation.y();
		elements[14] = translation.z();
	}

	void rotation(float angle, const Vector3d& axis) {
		float cosa = cosf(angle);
		float sina = sinf(angle);
		float x = axis.x();
		float y = axis.y();
		float z = axis.z();
		elements[0] = cosa + (1.0f - cosa) * x * x;
		elements[1] = (1.0f - cosa) * x * y + sina * z;
		elements[2] = (1.0f - cosa) * x * z - sina * y;
		elements[3] = 0.0f;
		elements[4] = (1.0f - cosa) * x * y - z * sina;
		elements[5] = cosa + (1.0f - cosa) * y * y;
		elements[6] = (1.0f - cosa) * y * z + x * sina;
		elements[7] = 0.0f;
		elements[8] = (1.0f - cosa) * x * z + sina * y;
		elements[9] = (1.0f - cosa) * y * z - sina * x;
		elements[10] = cosa + (1.0f - cosa) * z * z;
		elements[11] = 0.0f;
 		elements[12] = 0.0f;
		elements[13] = 0.0f;
		elements[14] = 0.0f;
		elements[15] = 1.0f;
	}

	void scale(const Vector3d& scale) {
		elements[1] = elements[2] = elements[3] = elements[4] = 0.0f;
		elements[6] = elements[7] = elements[8] = elements[9] = 0.0f;
		elements[11] = elements[12] = elements[13] = elements[14] = 0.0f;
		elements[15] = 1.0f;
		elements[0] =  scale.x();
		elements[5] =  scale.y();
		elements[10] = scale.z();
	}

	void ortho(float left, float right, float top, float bottom, float fnear, float ffar) {
		elements[0] = 2.0f / (right - left);
		elements[1] = 0.0f;
		elements[2] = 0.0f;
		elements[3] = 0.0f;
		elements[4] = 0.0f;
		elements[5] = 2.0f / (top - bottom);
		elements[6] = 0.0f;
		elements[7] = 0.0f;
		elements[8] = 0.0f;
		elements[9] = 0.0f;
		elements[10] = -2.0f / (ffar - fnear);
		elements[11] = 0.0f;
		elements[12] = -(left + right)  / (right - left);
		elements[13] = -(top + bottom) / (top - bottom);
		elements[14] = -(ffar + fnear) / (ffar - fnear);
		elements[15] = 1.0f;
	}

	void lookAt(Vector3d& eye, Vector3d& center) {
		Vector3d up(0.0f, 1.0f, 0.0f);
		float  x0, x1, x2, y0, y1, y2, z0, z1, z2, len,
			   eyex = eye.elements[0],
			   eyey = eye.elements[1],
			   eyez = eye.elements[2],
			   upx = up.elements[0],
			   upy = up.elements[1],
			   upz = up.elements[2],
			   centerx = center.elements[0],
			   centery = center.elements[1],
			   centerz = center.elements[2];
		if(eyex == centerx && eyey == centery && eyez == centerz) {
			identity();
			return;
		}
		//vec3.direction(eye, center, z);
		z0 = eyex - centerx;
		z1 = eyey - centery;
		z2 = eyez - centerz;
		// normalize (no check needed for 0 because of early return)
		len = 1.0f / sqrt(z0 * z0 + z1 * z1 + z2 * z2);
		z0 *= len;
		z1 *= len;
		z2 *= len;
		//vec3.normalize(vec3.cross(up, z, x));
		x0 = upy * z2 - upz * z1;
		x1 = upz * z0 - upx * z2;
		x2 = upx * z1 - upy * z0;
		len = sqrtf(x0 * x0 + x1 * x1 + x2 * x2);
		if(!len) {
			x0 = 0;
			x1 = 0;
			x2 = 0;
		} else {
			len = 1.0f / len;
			x0 *= len;
			x1 *= len;
			x2 *= len;
		}
		//vec3.normalize(vec3.cross(z, x, y));
		y0 = z1 * x2 - z2 * x1;
		y1 = z2 * x0 - z0 * x2;
		y2 = z0 * x1 - z1 * x0;
		len = sqrtf(y0 * y0 + y1 * y1 + y2 * y2);
		if(!len) {
			y0 = 0;
			y1 = 0;
			y2 = 0;
		} else {
			len = 1.0f / len;
			y0 *= len;
			y1 *= len;
			y2 *= len;
		}
		elements[0] = x0;
		elements[1] = y0;
		elements[2] = z0;
		elements[3] = 0;
		elements[4] = x1;
		elements[5] = y1;
		elements[6] = z1;
		elements[7] = 0;
		elements[8] = x2;
		elements[9] = y2;
		elements[10] = z2;
		elements[11] = 0;
		elements[12] = -(x0 * eyex + x1 * eyey + x2 * eyez);
		elements[13] = -(y0 * eyex + y1 * eyey + y2 * eyez);
		elements[14] = -(z0 * eyex + z1 * eyey + z2 * eyez);
		elements[15] = 1;
		return;
	}

	/* void lookAt(const Vector3d& at, Vector3d& direction) { */
	/* 	Vector3d k(0.0f, 1.0f, 0.0f); */
	/* 	direction.normalise(); */
	/* 	Vector3d up(k - (direction * dot(k, direction))); */
	/* 	up.normalise(); */
	/* 	Vector3d side(cross(direction, up)); */
	/* 	side.normalise(); */
	/* 	elements[0] = side.elements[0]; */
	/* 	elements[1] = up.elements[0]; */
	/* 	elements[2] = -direction.elements[0]; */
	/* 	elements[3] = 0.0f; */
	/* 	elements[4] = side.elements[1]; */
	/* 	elements[5] = up.elements[1]; */
	/* 	elements[6] = -direction.elements[1]; */
	/* 	elements[7] = 0.0f; */
	/* 	elements[8] = side.elements[2]; */
	/* 	elements[9] = up.elements[2]; */
	/* 	elements[10] = -direction.elements[2]; */
	/* 	elements[11] = 0.0f; */
	/* 	elements[12] = -dot(side, at); */
	/* 	elements[13] = -dot(up, at); */
	/* 	elements[14] = -dot(direction, at); */
	/* 	elements[15] = 1.0f; */
	/* } */

	/* void lookAt(const Vector3d& at, float x, float y, float z) { */
	/* 	Vector3d direction(x - at.x(), y - at.y(), z - at.z()); */
	/* 	lookAt(at, direction); */
	/* } */

	void frustum(float left, float right, float bottom, float top, float fnear, float ffar) {
		float rl = (right - left);
		float tb = (top - bottom);
		float fn = (ffar - fnear);
		elements[0] = (fnear * 2.0f) / rl;
		elements[1] = 0.0f;
		elements[2] = 0.0f;
		elements[3] = 0.0f;
		elements[4] = 0.0f;
		elements[5] = (fnear * 2.0f) / tb;
		elements[6] = 0.0f;
		elements[7] = 0.0f;
		elements[8] = (right + left) / rl;
		elements[9] = (top + bottom) / tb;
		elements[10] = -(ffar + fnear) / fn;
		elements[11] = -1.0f;
		elements[12] = 0.0f;
		elements[13] = 0.0f;
		elements[14] = -(ffar * fnear * 2.0f) / fn;
		elements[15] = 0.0f;
		return;
	}

	void persp(float fovy, float aspect, float fnear, float ffar) {
		// divide by 360.0f rather than 180.0f because this is half fov
		float top = fnear * tanf(fovy * M_PI / 360.0);
		float  right = top * aspect;
		frustum(-right, right, -top, top, fnear, ffar);
	}


	Matrix44 operator*(const Matrix44& other)	{
		Matrix44 tmp;
		for(int row = 0; row < 4; row++) {
			for(int col = 0; col < 4; col++) {
				tmp.elements[row + col * 4] =
					elements[row + 0] * other.elements[ 0 + col * 4] +
					elements[row + 4] * other.elements[ 1 + col * 4] +
					elements[row + 8] * other.elements[ 2 + col * 4] +
					elements[row + 12] * other.elements[ 3 + col * 4];
			}
		}
		return tmp;
	}

	Vector3d operator*(const Vector3d& vec) {
		Vector3d tmp;
		tmp.x() =
			elements[0] * vec.x() +
			elements[4] *  vec.y() +
			elements[8] *  vec.z();
		tmp.y() =
			elements[1] * vec.x() +
			elements[5] * vec.y() +
			elements[9] * vec.z();
		tmp.z() =
			elements[2] * vec.x() +
			elements[6]  * vec.y() +
			elements[10] * vec.z();
		return tmp;
	}

	Vertex3d operator*(const Vertex3d& vec) {
		Vertex3d tmp;
		tmp.x() =
			elements[0] * vec.x() +
			elements[4] *  vec.y() +
			elements[8] *  vec.z() +
			elements[12] * vec.w();
		tmp.y() =
			elements[1] * vec.x() +
			elements[5] * vec.y() +
			elements[9]   * vec.z() +
			elements[13]  * vec.w();
		tmp.z() =
			elements[2] * vec.x() +
			elements[6]  * vec.y() +
			elements[10] * vec.z() +
			elements[14] * vec.w();
		tmp.w() =
			elements[3] * vec.x() +
			elements[7]  * vec.y() +
			elements[11] * vec.z() +
			elements[15] * vec.w();
		return tmp;
	}

}; /* class Matrix44 */


}

#endif


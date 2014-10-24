#ifndef MATRIX44_H_INCLUDED
#define MATRIX44_H_INCLUDED

namespace venk
{

class Matrix44 : public Tuple<float, 16>
{
public:
	static const std::size_t tuple_size = 16;
	static const eScalarType element_type = BufferElementType<float>::element_type;

	/* construct, no initialisation */
	Matrix44() {
	}

	Matrix44(const Quat& q, const Vector3d& v)
	{

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

		elements[12] = v.elements[Vector3d::X];
		elements[13] = v.elements[Vector3d::Y];
		elements[14] = v.elements[Vector3d::Z];
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
		elements[11] = elements[12] = 0.0f;
		elements[0] = elements[5] = elements[10] = 1.0f;

		elements[13] = translation.elements[Vector3d::X];
		elements[14] = translation.elements[Vector3d::Y];
		elements[15] = translation.elements[Vector3d::Z];
	}

	void rotation(float angle, const Vector3d& axis) {
		float cosa = cosf(angle);
		float sina = sinf(angle);
		float x = axis.elements[Vector3d::X];
		float y = axis.elements[Vector3d::Y];
		float z = axis.elements[Vector3d::Z];

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

		elements[0] =  scale.elements[Vector3d::X];
		elements[5] =  scale.elements[Vector3d::Y];
		elements[10] = scale.elements[Vector3d::Z];
	}

	void ortho(float left, float right, float top, float bottom, float far, float near) {
		elements[0] = 2.0f / ( right - left );
		elements[1] = 0.0f;
		elements[2] = 0.0f;
		elements[3] = 0.0f;

		elements[4] = 0.0f;
		elements[5] = 2.0f / ( top - bottom );
		elements[6] = 0.0f;
		elements[7] = 0.0f;

		elements[8] = 0.0f;
		elements[9] = 0.0f;
		elements[10] = -2.0f / ( far - near );
		elements[11] = 0.0f;

		elements[12] = ( right + left )  / ( right - left );
		elements[13] = ( top + bottom ) / ( top - bottom );
		elements[14] = ( far + near ) / ( far - near );
		elements[15] = 1.0f;
	}


	void lookAt(const Vector3d& at, Vector3d& direction) {
		Vector3d k(0.0f, 1.0f, 0.0f);
		direction.normalise();
		Vector3d up(k - (direction * dot(k, direction)));
		up.normalise();
		Vector3d side(cross(direction, up));
		side.normalise();
		elements[0] = side.elements[0];
		elements[1] = up.elements[0];
		elements[2] = -direction.elements[0];
		elements[3] = 0.0f;
		elements[4] = side.elements[1];
		elements[5] = up.elements[1];
		elements[6] = -direction.elements[1];
		elements[7] = 0.0f;
		elements[8] = side.elements[2];
		elements[9] = up.elements[2];
		elements[10] = -direction.elements[2];
		elements[11] = 0.0f;
		elements[12] = -dot(side, at);
		elements[13] = -dot(up, at);
		elements[14] = -dot(direction, at);
		elements[15] = 1.0f;
	}

	void lookAt(const Vector3d& at, float x, float y, float z) {
		Vector3d direction(x - at.elements[Vector3d::X], y - at.elements[Vector3d::Y], z - at.elements[Vector3d::Z]);
		lookAt(at, direction);
	}

    void frustum(float left, float right, float bottom, float top, float near, float far) {
		float rl = (right - left);
		float tb = (top - bottom);
		float fn = (far - near);

		elements[0] = (near * 2.0f) / rl;
		elements[1] = 0.0f;
		elements[2] = 0.0f;
		elements[3] = 0.0f;
		elements[4] = 0.0f;
		elements[5] = (near * 2.0f) / tb;
		elements[6] = 0.0f;
		elements[7] = 0.0f;
		elements[8] = (right + left) / rl;
		elements[9] = (top + bottom) / tb;
		elements[10] = -(far + near) / fn;
		elements[11] = -1.0f;
		elements[12] = 0.0f;
		elements[13] = 0.0f;
		elements[14] = -(far * near * 2.0f) / fn;
		elements[15] = 0.0f;
		return;
	}

	void persp(float fovy, float aspect, float near, float far) {
		// divide by 360.0f rather than 180.0f because this is half fov
		float top = near * tanf(fovy * M_PI / 360.0);
		float  right = top * aspect;
		frustum(-right, right, -top, top, near, far);
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

        tmp.elements[Vector3d::X] =
            elements[0] * vec.elements[Vector3d::X] +
            elements[4] *  vec.elements[Vector3d::Y] +
            elements[8] *  vec.elements[Vector3d::Z];

        tmp.elements[Vector3d::Y] =
            elements[1] * vec.elements[Vector3d::X] +
            elements[5] * vec.elements[Vector3d::Y] +
            elements[9] * vec.elements[Vector3d::Z];

        tmp.elements[Vector3d::Z] =
            elements[2] * vec.elements[Vector3d::X] +
            elements[6]  * vec.elements[Vector3d::Y] +
            elements[10] * vec.elements[Vector3d::Z];

        return tmp;
    }

    Vertex3d operator*(const Vertex3d& vec) {

        Vertex3d tmp;

        tmp.elements[Vertex3d::X] =
            elements[0] * vec.elements[Vertex3d::X] +
            elements[4] *  vec.elements[Vertex3d::Y] +
            elements[8] *  vec.elements[Vertex3d::Z] +
            elements[12] * vec.elements[Vertex3d::W];

        tmp.elements[Vertex3d::Y] =
            elements[1] * vec.elements[Vertex3d::X] +
            elements[5] * vec.elements[Vertex3d::Y] +
            elements[9]   * vec.elements[Vertex3d::Z] +
            elements[13]  * vec.elements[Vertex3d::W];

        tmp.elements[Vertex3d::Z] =
            elements[2] * vec.elements[Vertex3d::X] +
            elements[6]  * vec.elements[Vertex3d::Y] +
            elements[10] * vec.elements[Vertex3d::Z] +
            elements[14] * vec.elements[Vertex3d::W];

        tmp.elements[Vertex3d::W] =
            elements[3] * vec.elements[Vertex3d::X] +
            elements[7]  * vec.elements[Vertex3d::Y] +
            elements[11] * vec.elements[Vertex3d::Z] +
            elements[15] * vec.elements[Vertex3d::W];

        return tmp;
    }

}; /* class Matrix44 */


}

#endif

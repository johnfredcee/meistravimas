#ifndef MATRIX44_H_INCLUDED
#define MATRIX44_H_INCLUDED

namespace venk
{

class Matrix44 : public Tuple<float, 16>
{
public:
	static const std::size_t tuple_size = 16;
	static const eScalarType element_type = BufferElementType<float>::element_type;

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
		identity();
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
		zero();
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

	void view(const Vector3d& at, Vector3d& view) {
		Vector3d k(0.0f, 1.0f, 0.0f);
		view.normalise();
		Vector3d up(k - (view * dot(k, view)));
		up.normalise();
		Vector3d side(cross(view, up));
		side.normalise();
		elements[0] = side.elements[0];
		elements[1] = up.elements[0];
		elements[2] = -view.elements[0];
		elements[3] = 0.0f;
		elements[4] = side.elements[1];
		elements[5] = up.elements[1];
		elements[6] = -view.elements[1];
		elements[7] = 0.0f;
		elements[8] = side.elements[2];
		elements[9] = up.elements[2];
		elements[10] = -view.elements[2];
		elements[11] = 0.0f;
		elements[12] = -dot(side, at);
		elements[13] = -dot(up, at);
		elements[14] = -dot(view, at);
		elements[15] = 1.0f;
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

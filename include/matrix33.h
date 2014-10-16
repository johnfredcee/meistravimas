#ifndef MATRIX33_H_INCLUDED
#define MATRIX33_H_INCLUDED

namespace venk
{
class  Matrix33 : public Tuple<float, 9>
{
	static const std::size_t tuple_size = 9;
	static const eScalarType element_type = BufferElementType<float>::gl_type;

	void identity() {
		zero();
		elements[0] = elements[4] = elements[8] = 1.0f;
	}

	void transpose() {
		Matrix33 tmp;
		for(int i = 0; i < 9; i++) {
			tmp.elements[i] = elements[ i / 3 + (i % 3) * 3 ];
		}
		copy(tmp);
	}

	void translation(float angle, const Vector2d& translation) {
		identity();
		elements[6] = translation.elements[Vector2d::X];
		elements[7] = translation.elements[Vector2d::Y];
	}


	void rotation(float angle) {
		float cosa = cosf(angle);
		float sina = sinf(angle);

		identity();
		elements[0] = cosa;
		elements[1] = sina;
		elements[4] = -sina;
		elements[5] = cosa;
	}

	void scale(const Vector2d& scale) {
		zero();
		elements[0] = scale.elements[Vector2d::X];
		elements[4] = scale.elements[Vector2d::Y];
		elements[8] = 1.0f;
	}


}; /* class Matrix33 */
}
#endif

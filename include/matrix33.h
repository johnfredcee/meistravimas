#ifndef MATRIX33_H_INCLUDED
#define MATRIX33_H_INCLUDED

namespace venk {
class  Matrix33 : public Tuple<float, 9> {
	static const std::size_t tuple_size = 9;
	static const eScalarType element_type = BufferElementType<float>::gl_type;

	/* construct. No initialisation */
	Matrix33() {}

	/* construct matrix from quat */
	Matrix33(const Quat& q) {
		float x = q.elements[Quat::X];
		float y = q.elements[Quat::Y];
		float z = q.elements[Quat::Z];
		float w = q.elements[Quat::W];
		x2 = x + x,
		y2 = y + y,
		z2 = z + z,
		xx = x * x2,
		xy = x * y2,
		xz = x * z2,
		yy = y * y2,
		yz = y * z2,
		zz = z * z2,
		wx = w * x2,
		wy = w * y2,
		wz = w * z2;
		elements[0] = 1 - (yy + zz);
		elements[1] = xy + wz;
		elements[2] = xz - wy;
		elements[3] = xy - wz;
		elements[4] = 1 - (xx + zz);
		elements[5] = yz + wx;
		elements[6] = xz + wy;
		elements[7] = yz - wx;
		elements[8] = 1 - (xx + yy);
		return; }

	/* identity matrix */
	void identity() {
		elements[0] = 1.0f;
		elements[1] = 0.0f;
		elements[2] = 0.0f;
		elements[3] = 0.0f;
		elements[4] = 1.0f;
		elements[5] = 0.0f;
		elements[6] = 0.0f;
		elements[7] = 0.0f;
		elements[8] = 1.0f; }

	/* transpose */
	void transpose() {
		Matrix33 tmp;
		for(int i = 0; i < 9; i++) {
			float tmp = elements[i];
			elements[i] = elements[ i / 3 + (i % 3) * 3 ];
			elements[i / 3 + (i % 3) * 3 ] = tmp; } }


	/* 2d transation */
	void translation(const Vector2d& translation) {
		elements[0] = 1.0f;
		elements[1] = 0.0f;
		elements[2] = 0.0f;
		elements[3] = 0.0f;
		elements[4] = 1.0f;
		elements[5] = 0.0f;
		elements[6] = translation.elements[Vector2d::X];
		elements[7] = translation.elements[Vector2d::Y];
		elements[8] = 1.0f; }


	/* 2d rotation */
	void rotation(float angle) {
		float cosa = cosf(angle);
		float sina = sinf(angle);
		elements[0] = cosa;
		elements[1] = sina;
		elements[2] = 0.0f;
		elements[3] = 0.0f;
		elements[4] = -sina;
		elements[5] = cosa;
		elements[6] = 0.0f;
		elements[7] = 0.0f;
		elements[8] = 1.0f; }


	/* 2d scale */
	void scale(const Vector2d& scale) {
		elements[0] = scale.elements[Vector2d::X];
		elements[1] = 0.0f;
		elements[2] = 0.0f;
		elements[3] = 0.0f;
		elements[4] = scale.elements[Vector2d::Y];
		elements[5] = 0.0f;
		elements[6] = 0.0f;
		elements[7] = 0.0f;
		elements[8] = 1.0f; }


	Vector2d operator*(const Vector2d& vec) {
		Vector2d tmp;
		tmp.x() =
			elements[0] * vec.x() +
			elements[3] * vec.y();
		tmp.y() =
			elements[1] * vec.x() +
			elements[4] * vec.y();
		return tmp; }

	Vertex2d operator*(const Vertex2d& vec) {
		Vertex2d tmp;
		tmp.x() =
			elements[0] * vec.x() +
			elements[3] * vec.y() +
			elements[6] * vec.w();
		tmp.y() =
			elements[1] * vec.x() +
			elements[4] * vec.y() +
			elements[7] * vec.w();
		tmp.w() =
			elements[2] * vec.x() +
			elements[5] * vec.y() +
			elements[8] * vec.w();
		return tmp; }


}; /* class Matrix33 */




}
#endif


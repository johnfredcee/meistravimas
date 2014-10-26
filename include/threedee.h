#ifndef THREE_DEE_H_INCLUDED
#define THREE_DEE_H_INCLUDED

namespace venk {
class Matrix44;

class Vector3d : public Tuple<float, 3> {
private:
	static const int X = 0;
	static const int Y = 1;
	static const int Z = 2;
public:
	static const std::size_t tuple_size = 3;
	static const eScalarType element_type = BufferElementType<float>::element_type;

	Vector3d() {}

	Vector3d(float x, float y, float z) {
		elements[X] = x;
		elements[Y] = y;
		elements[Z] = z; }

	float magnitude() const {
		return sqrtf(elements[X] * elements[X] +
					 elements[Y] * elements[Y] +
					 elements[Z] * elements[Z]); }


	Vector3d& normalise() {
		float mag = magnitude();
		elements[X] /= mag;
		elements[Y] /= mag;
		elements[Z] /= mag;
		return *this; }



	inline float& x() { return elements[X]; };
	inline float x()  const { return elements[X]; };
	inline float& y()  { return elements[Y]; };
	inline float y() const { return elements[Y]; };
	inline float& z()  { return elements[Z]; };
	inline float z() const { return elements[Z]; };

};

Vector3d operator*(const Vector3d& v, float scalar);

float dot(const Vector3d& a, const Vector3d& b);

Vector3d cross(const Vector3d& a, const Vector3d& b);

Vector3d operator-(const Vector3d& a, const Vector3d& b);

class Vertex3d : public Tuple<float, 4> {
private:
	static const int X = 0;
	static const int Y = 1;
	static const int Z = 2;
	static const int W = 3;
public:
	static const std::size_t tuple_size = 4;
	static const eScalarType element_type = BufferElementType<float>::element_type;

	Vertex3d() {
		elements[W] = 1.0f; }

	Vertex3d(const Vector3d& other) {
		elements[X] = other.elements[X];
		elements[Y] = other.elements[Y];
		elements[Z] = other.elements[Z]; }

	Vertex3d(float x, float y, float z) {
		elements[X] = x;
		elements[Y] = y;
		elements[Z] = z;
		elements[W] = 1.0f; }

	inline float& x() { return elements[X]; };
	inline float x()  const { return elements[X]; };
	inline float& y()  { return elements[Y]; };
	inline float y() const { return elements[Y]; };
	inline float& z() { return elements[Z]; };
	inline float z() const { return elements[Z]; };
	inline float& w() { return elements[W]; };
	inline float w() const { return elements[W]; };

};


}

#endif





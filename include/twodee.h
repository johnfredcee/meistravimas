#ifndef TWO_DEE_H_INCLUDED
#define TWO_DEE_H_INCLUDED

namespace venk {
class Matrix33;

class Vector2d : public Tuple<float, 2> {
private:
	static const int X = 0;
	static const int Y = 1;
	static const int U = 0;
	static const int V = 1;
public:
	static const std::size_t tuple_size = 2;
	static const eScalarType element_type = BufferElementType<float>::element_type;

	Vector2d(float x, float y) {
		elements[X] = x;
		elements[Y] = y; }

	float magnitude() const {
		return sqrtf(elements[X] * elements[X] +
					 elements[Y] * elements[Y]); }

	inline float& x() { return elements[X]; };
	inline float x()  const { return elements[X]; };
	inline float& y()  { return elements[Y]; };
	inline float y() const { return elements[Y]; };

	inline float& u() { return elements[U]; };
	inline float u()  const { return elements[U]; };
	inline float& v()  { return elements[V]; };
	inline float v() const { return elements[V]; };


};

inline float dot(const Vector2d& a, const Vector2d& b)  {
	return a.x() * b.x() +
		a.y() * b.y(); }

class Vertex2d : public Tuple<float, 3> {
public:
	static const int X = 0;
	static const int Y = 1;
	static const int W = 2;

	static const std::size_t tuple_size = 3;
	static const eScalarType element_type = BufferElementType<float>::element_type;

};

}
#endif


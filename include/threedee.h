#ifndef THREE_DEE_H_INCLUDED
#define THREE_DEE_H_INCLUDED

namespace venk
{
    class Matrix44;

    class Vector3d : public Tuple<float, 3>
    {
        public:
            static const int X = 0;
            static const int Y = 1;
            static const int Z = 2;

            static const std::size_t tuple_size = 3;
            static const eScalarType element_type = BufferElementType<float>::element_type;

			Vector3d()
			{
			}

            Vector3d(float x, float y, float z) {
                elements[X] = x;
                elements[Y] = y;
                elements[Z] = z;    
            }

            float magnitude() const {
                return sqrtf(elements[X] * elements[X] +
                             elements[Y] * elements[Y] +
                             elements[Z] * elements[Z]);
            }

   
            Vector3d& normalise() {
                float mag = magnitude();
                elements[X] /= mag;
                elements[Y] /= mag;
                elements[Z] /= mag;
                return *this;
            }


    };

	Vector3d operator*(const Vector3d& v, float scalar) 
	{
		return Vector3d(v.elements[Vector3d::X] * scalar, v.elements[Vector3d::Y] * scalar, v.elements[Vector3d::Z] * scalar);
	}
	
	Vector3d operator+(const Vector3d& v, float scalar) 
	{
		return Vector3d(v.elements[Vector3d::X] + scalar, v.elements[Vector3d::Y] + scalar, v.elements[Vector3d::Z] + scalar);
	}

	float dot(const Vector3d& a, const Vector3d& b)  {
		return (a.elements[Vector3d::X] * b.elements[Vector3d::X]) +
			(a.elements[Vector3d::Y] * b.elements[Vector3d::Y]) +
			(a.elements[Vector3d::Z] * b.elements[Vector3d::Z]);
    }

	Vector3d cross(const Vector3d& a, const Vector3d& b) {
		Vector3d tmp;
		tmp.elements[Vector3d::X] = a.elements[Vector3d::Y] * b.elements[Vector3d::Z] - a.elements[Vector3d::Z] * b.elements[Vector3d::Y];
		tmp.elements[Vector3d::Y] = a.elements[Vector3d::Z] * b.elements[Vector3d::X] - a.elements[Vector3d::X] * b.elements[Vector3d::Z];
		tmp.elements[Vector3d::Z] = a.elements[Vector3d::X] * b.elements[Vector3d::Y] - a.elements[Vector3d::Y] * b.elements[Vector3d::X];
		return tmp;
	}

	Vector3d operator-(const Vector3d& a, const Vector3d& b) 
	{
		Vector3d tmp(a.elements[Vector3d::X] - b.elements[Vector3d::X], 
					 a.elements[Vector3d::Y] - b.elements[Vector3d::Y],
					 a.elements[Vector3d::Z] - b.elements[Vector3d::Z]);
		return tmp;
	}

    class Vertex3d : public Tuple<float, 4>
    {
        public:
            static const int X = 0;
            static const int Y = 1;
            static const int Z = 2;
            static const int W = 3;

            static const std::size_t tuple_size = 4;
            static const eScalarType element_type = BufferElementType<float>::element_type;

			Vertex3d() 
			{
                elements[W] = 1.0f;
			}

            Vertex3d(const Vector3d& other)
            {
                elements[X] = other.elements[X];
                elements[Y] = other.elements[Y];
                elements[Z] = other.elements[Z];
            }

            Vertex3d(float x, float y, float z)
            {
                elements[X] = x;
                elements[Y] = y;
                elements[Z] = z;
                elements[W] = 1.0f;
            }


    };

 
}

#endif

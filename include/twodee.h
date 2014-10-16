#ifndef TWO_DEE_H_INCLUDED
#define TWO_DEE_H_INCLUDED

namespace venk
{
    class Matrix33;

    class Vector2d : public Tuple<float, 2>
    {
        public:
            static const int X = 0;
            static const int Y = 1;
            static const int U = 0;
            static const int V = 1;

            static const std::size_t tuple_size = 2;
            static const eScalarType element_type = BufferElementType<float>::element_type;            

            Vector2d(float x, float y) {
                elements[X] = x;
                elements[Y] = y;
            }

            float magnitude() const {
                return sqrtf(elements[X] * elements[X] +
                             elements[Y] * elements[Y]);

            }


    };

    float dot(const Vector2d& a, const Vector2d& b) const {
                return a.elements[X] * b.elements[X] +
                       a.elements[Y] * b.elements[Y];
    }

    class Vertex2d : public Tuple<float, 3>
    {
        public:
            static const int X = 0;
            static const int Y = 1;
            static const int W = 2;

            static const std::size_t tuple_size = 3;
            static const eScalarType element_type = BufferElementType<float>::element_type;

    };

    

    Vector2d operator*(const Matrix33& m33, const Vector2d& vec) {
        Vector2d tmp;

        tmp.elements[Vector2d::X] =
                m33.elements[0] * vec.elements[Vector2d::X] +
                m33.elements[3] * vec.elements[Vector2d::Y];

        tmp.elements[Vector2d::Y] =
                m33.elements[1] * vec.elements[Vector2d::X] +
                m33.elements[4] * vec.elements[Vector2d::Y];

        return tmp;
    }

    Vertex2d operator*(const Matrix33& m33, const Vertex2d& vec) {
        Vertex2d tmp;

        tmp.elements[Vertex2d::X] =
                m33.elements[0] * vec.elements[Vertex2d::X] +
                m33.elements[3] * vec.elements[Vertex2d::Y] +
                m33.elements[6] * vec.elements[Vertex2d::W];
        tmp.elements[Vertex2d::Y] =
                m33.elements[1] * vec.elements[Vertex2d::X] +
                m33.elements[4] * vec.elements[Vertex2d::Y] +
                m33.elements[7] * vec.elements[Vertex2d::W];
        tmp.elements[Vertex2d::W] =
                m33.elements[2] * vec.elements[Vertex2d::X] +
                m33.elements[5] * vec.elements[Vertex2d::Y] +
                m33.elements[8] * vec.elements[Vertex2d::W];

        return tmp;
    }  

}
#endif

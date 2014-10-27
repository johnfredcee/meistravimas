#ifndef COLOUR_H
#define COLOUR_H

namespace venk
{

	class ColorRGBA : public Tuple<float, 4>
	{
	public:
		static const int R = 0;
		static const int G = 1;
		static const int B = 2;		
		static const int A = 3;

		static const std::size_t tuple_size = 4;
		static const eScalarType element_type = BufferElementType<float>::element_type;

		ColorRGBA() {
		}

		ColorRGBA(float r, float g, float b, float a = 0.0f) {
			elements[R] = r;
			elements[G] = g;    
			elements[B] = b;    
			elements[A] = a;
		}
	};
}
#endif

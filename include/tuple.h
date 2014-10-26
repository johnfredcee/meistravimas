
#ifndef TUPLE_H_INCLUDED
#define TUPLE_H_INCLUDED

#include <cfloat>
#include <cmath>

namespace venk
{
	enum eScalarType
	{
		UNSIGNED_BYTE,
		SIGNED_BYTE,
		UNSIGNED_SHORT,
		SIGNED_SHORT,
		UNSIGNED_INT,
		SIGNED_INT,
		FLOAT,
		DOUBLE
	};

	/**
	 * Types of element that go in buffer that gets sent to GPU
	 */
	template <typename T> class BufferElementType
	{
	};

	template <> class BufferElementType<Uint8>
	{
	public:
		static const eScalarType element_type = UNSIGNED_BYTE;
	};

	template <> class BufferElementType<Sint8>
	{
	public:
		static const eScalarType element_type = SIGNED_BYTE;
	};

	template <> class BufferElementType<Uint16>
	{
	public:
		static const eScalarType element_type = UNSIGNED_SHORT;
	};

	template <> class BufferElementType<Sint16>
	{
	public:
		static const eScalarType element_type = SIGNED_SHORT;
	};

	template <> class BufferElementType<Uint32>
	{
	public:
		static const eScalarType element_type = UNSIGNED_INT;
	};

	template <> class BufferElementType<Sint32>
	{
	public:
		static const eScalarType element_type = SIGNED_INT;
	};

	template <> class BufferElementType<float>
	{
	public:
		static const eScalarType element_type = FLOAT;
	};

	template <> class BufferElementType<double>
	{
	public:
		static const eScalarType element_type = DOUBLE;
	};


	/**
	 * POD tuple type with N elements of type T which can be used as a
	 * basis for vectors, etc. Pretty much obsolete as now in C++11
	 */
	template <typename T, std::size_t N> class Tuple
	{

	public:
		T elements[N];

		static const std::size_t tuple_size = N;
		static const eScalarType element_type = BufferElementType<T>::element_type;

		void zero()
		{
			memset((void*) elements, 0, sizeof(elements));
		};

		void copy(const Tuple<T,N>& other)
		{
			memcpy((void*) elements, (void*) other.elements, sizeof(elements));
		};

		T& operator*=(T scale)
		{
			for(int i = 0; i < N; i++)
			{
				elements[N] *= scale;
			}
			return *this;
		}

		T& operator/=(T scale)
		{
			T recipr = 1.0f / scale;


			for(int i = 0; i < N; i++)
			{
				elements[N] *= recipr;
			}
			return *this;
		}

		void operator+=(T sum)
		{
			for(int i = 0; i < N; i++)
			{
				elements[N] += sum;
			}
		}

		T& operator-=(T diff)
		{
			for(int i = 0; i < N; i++)
			{
				elements[N] -= diff;
			}
			return *this;
		}

		T& operator()(std::size_t i)
		{
			return elements[i];
		}

		const T& operator()(std::size_t i) const
		{
			return elements[i];
		}

		template <typename Q>	
			inline float& U() { return elements[Q::U]; };

		template <typename Q> 
			inline float& V() { return elements[Q::V]; };

	};



};

#endif

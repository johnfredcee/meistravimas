#ifndef POOL_H_INCLUDED
#define POOL_H_INCLUDED

#include <eastl/vector.h>

template <typename I>
class Pool
{
public:
	typedef I value_type;
	typedef typename eastl::vector<I>::size_type size_type;
	typedef typename eastl::vector<I>::reference reference;
	typedef typename eastl::vector<I>::const_reference const_reference;

private:
	eastl::vector<I> 			objects;
	eastl::vector<size_type> 	free;
	size_type                   count;

	Pool();
	bool empty() const;
	bool size() const;
    void clear();
    size_type add(const value_type& value);
    void remove(const size_type n);
    reference       operator[](size_type n);
    const_reference operator[](size_type n) const;   
};

template<typename I>
inline Pool<I>::Pool()
{
	return;
}

template<typename I>
inline bool Pool<I>::empty() const
{
	return objects.size() == free.size();
}

template<typename I>
inline void Pool<I>::clear()
{
	free.clear();
	objects.clear();
}

template<typename I>
inline typename Pool<I>::size_type Pool<I>::add(const typename Pool<I>::value_type& value)
{
	typename Pool<I>::size_type result = 0;
	if (!free.empty()) {
		result = free.back();
		objects[result] = value;
		free.pop_back();
	} else {
		objects.push_back(value);
		result = objects.size();
	}
	return result;
}

template<typename I>
inline void Pool<I>::remove(const typename Pool<I>::size_type n)
{
	free.push_back(n);
	return;
}

template<typename I>
inline typename Pool<I>::reference 
Pool<I>::operator[](typename Pool<I>::size_type n)
{
	return objects.at(n);
}

template<typename I>
inline typename Pool<I>::const_reference 
Pool<I>::operator[](typename Pool<I>::size_type n) const
{
	return objects.at(n);
}

#endif
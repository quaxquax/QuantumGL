#ifndef _H_DYNARRAY
#define _H_DYNARRAY

#include <stdlib.h>

template <class T> class DynArray
{
public:
	int	n;
	T*	x;
	
	int	size;

private:
	inline void resize(int nsiz)
	{
		x = (T*) realloc(x,nsiz*sizeof(T));
		size = nsiz;
	}
	
public:
    // Array subscript operators
    inline T& operator[](int index) {
        return x[index];
    }
    
    inline const T& operator[](int index) const {
        return x[index];
    }
    
	inline void fit()
	{
		resize(n);
	}
	
	inline void allocate(int nnew)
	{
		int nsiz = size;
		while(nsiz < n+nnew)
			nsiz *= 2;
		if(nsiz != size)
			resize(nsiz);
	}
	
	inline void append(T what)
	{
		allocate(1);
		x[n++] = what;
	}
	
	DynArray()
	{
		size = 16;
		n = 0;
		x = (T*) malloc(size*sizeof(T));
	}
	
	~DynArray()
	{
		free(x);
	}
};

#endif

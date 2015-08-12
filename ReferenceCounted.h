#ifndef _H_REFERENCE_COUNTED
#define _H_REFERENCE_COUNTED


class AutoreleasePool;

class ReferenceCounted
{
	int		refCount;
public:
			ReferenceCounted();
	virtual	~ReferenceCounted();

	void	IncRefCount();
	void	DecRefCount();
	void	AutoRelease(AutoreleasePool *pool = 0);
};

	// utility functions:
	// T*	retain(T*)
	// T*	autorelease(T*);
	// void release(T*);

// T derrived from ReferenceCounted
template <class T> T* autorelease(T* x, AutoreleasePool *pool = 0)
{
	if(x)
		x->AutoRelease(pool);
	return x;
}

template <class T> T* retain(T* x)
{
	if(!x)
		return x;
	x->IncRefCount();
	return x;
}

inline void release(ReferenceCounted *x)
{
	if(x) x->DecRefCount();
}

void PrintReferenceCountingStatistics();

#endif

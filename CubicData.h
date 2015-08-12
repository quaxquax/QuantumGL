#ifndef _H_CUBIC_DATA
#define _H_CUBIC_DATA

#include "QuantumMath.h"
#include "ReferenceCounted.h"

template <class T> class CubicData
{
public:
	int		cx, cy, cz, n;
	T ***data;
	
			CubicData(int x = 0, int y = 0, int z = 0, int n = 0)
				: data(0)
			{
				Allocate(x,y,z,n);
			}
			
			~CubicData()
			{
				Deallocate();
			}
			
	void	Allocate(int x, int y, int z, int nn = 1)
			{
				Deallocate();
				if(x == 0)
				{
					data = 0;
					return;
				}
				cx = x, cy = y; cz = z; n = nn;
				data = new T** [cx];
				for(int i=0;i<cx;i++)
				{
					data[i] = new T* [cy];
					for(int j=0;j<cy;j++)
						data[i][j] = new T [cz*n];
				}
			}
			
	void	Deallocate()
			{
				if(!data)
					return;
				for(int i=0;i<cx;i++)
				{
					for(int j=0;j<cy;j++)
						delete [] data[i][j];
					delete [] data[i];
				}
				delete [] data;
				data = 0;
			}
};

class GeneralCubicData : public ReferenceCounted
{
public:
	CubicData<number>					*R;
	CubicData< complex<number> >		*C;
	
	number	min, max;
	int		cx, cy, cz;
	
	void	UpdateInfo(number min = 0, number max = 0)
			{
				this->min = min, this->max = max;
				if(R)
					cx = R->cx, cy = R->cy, cz = R->cz;
				else
					cx = C->cx, cy = C->cy, cz = C->cz;
			}
	
			GeneralCubicData()
				 : R(NULL), C(NULL)/*, refCount(1)*/ {}
			GeneralCubicData(CubicData<number> *r)
				 : R(r), C(NULL)/*, refCount(1)*/ {}
			GeneralCubicData(CubicData< complex<number> > *c)
				 : R(NULL), C(c)/*, refCount(1)*/ {}
	
	/*void	IncRefCount()	{ refCount++; }
	void	DecRefCount()	{ if(--refCount <= 0) delete this; }*/
private:
	virtual	~GeneralCubicData()
			{
				if(R) delete R;
				if(C) delete C;
			}
	//int									refCount;
	
	friend	void MyDearFriendGCC();
		// GCC produces a useless warning:
		// CubicData.h:90: warning: `class GeneralCubicData' 
		// only defines a private destructor and has no friends
		// ... so what?
};

#endif

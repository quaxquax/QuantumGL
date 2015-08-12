#ifndef _H_POINTLIST
#define _H_POINTLIST

#include "ReferenceCounted.h"
#include "QuantumMath.h"

class PointList : public ReferenceCounted
{
public:
	virtual void	StartIterating() = 0;
	virtual bool	GetNextPoint(vecR3& point) = 0;
};

// vectors dirField color colorField 
//		at volume_grid spacing 0.1
//		at slice x = 0 spacing 0.1
#endif

#ifndef _H_GRIDPOINTLIST
#define _H_GRIDPOINTLIST

#include "PointList.h"

class ValueEvaluator;

class GridPointList : public PointList
{
	ValueEvaluator	*cornerE,*deltaE,*countE;
	bool			havePoint;
	vecR3			corner, delta;
	vecN3			count, current;
public:
					GridPointList(ValueEvaluator* corner,ValueEvaluator* delta,ValueEvaluator* count);
					~GridPointList();

	virtual void	StartIterating();
	virtual bool	GetNextPoint(vecR3& point);
};

#endif

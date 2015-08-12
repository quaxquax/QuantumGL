#ifndef _H_EXPLICITPOINTLIST
#define _H_EXPLICITPOINTLIST

#include "PointList.h"

#include <list>

class ValueEvaluator;

class ExplicitPointList : public PointList
{
	std::list<ValueEvaluator*>
					points;
	std::list<ValueEvaluator*>::iterator it;
public:
					ExplicitPointList();
					~ExplicitPointList();

			void	AddPoint(ValueEvaluator* p);

	virtual void	StartIterating();
	virtual bool	GetNextPoint(vecR3& point);
};

#endif

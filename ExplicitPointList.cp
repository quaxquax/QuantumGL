#include "ExplicitPointList.h"
#include "ValueEvaluator.h"

ExplicitPointList::ExplicitPointList()
{
	it = points.end();
}
ExplicitPointList::~ExplicitPointList()
{
	for(std::list<ValueEvaluator*>::iterator p = points.begin();
		p != points.end();
		++p)
		release(*p);
}

void ExplicitPointList::AddPoint(ValueEvaluator* p)
{
	points.push_back(retain(p));
}

void ExplicitPointList::StartIterating()
{
	it = points.begin();
}

bool ExplicitPointList::GetNextPoint(vecR3& p)
{
	if(it == points.end())
		return false;
	
	(*it)->EvaluateReal(3,&p.x);
	++it;
	return true;
}

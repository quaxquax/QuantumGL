#include "GridPointList.h"
#include "ValueEvaluator.h"

GridPointList::GridPointList(ValueEvaluator* corner, ValueEvaluator* delta, ValueEvaluator* count)
	: cornerE(retain(corner)), deltaE(retain(delta)), countE(retain(count))
{
}

GridPointList::~GridPointList()
{
	release(cornerE);
	release(deltaE);
	release(countE);
}
	
void GridPointList::StartIterating()
{
	cornerE->EvaluateReal(3,&corner.x);
	deltaE->EvaluateReal(3,&delta.x);
	number tmp[3];
	countE->EvaluateReal(3,tmp);
	count.x = (int) (tmp[0] + 0.5);
	count.y = (int) (tmp[1] + 0.5);
	count.z = (int) (tmp[2] + 0.5);
	current = nullVecN3;
}

bool GridPointList::GetNextPoint(vecR3& point)
{
	if(current.x < count.x && current.y < count.y && current.z < count.z)
	{
		point.x = corner.x + current.x * delta.x;
		point.y = corner.y + current.y * delta.y;
		point.z = corner.z + current.z * delta.z;

		if(++current.x >= count.x)
			if(current.x = 0, ++current.y >= count.y)
				current.y = 0, ++current.z; // sorry about the commas... ;-)

		return true;
	}
	else	
		return false;
}

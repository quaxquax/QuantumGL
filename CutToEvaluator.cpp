#include "CutToEvaluator.h"
#include <assert.h>

CutToEvaluator::CutToEvaluator(ExpressionEvaluator *p, Range xr, Range yr, Range zr)
	: param(retain(p))
{
	assert(p);
	xRange = xr;
	yRange = yr;
	zRange = zr;

	Range fromXRange, fromYRange, fromZRange;
	p->GetRanges(fromXRange,fromYRange,fromZRange);

	assert(xr.inside(fromXRange));	// #### real error handling please
	assert(yr.inside(fromYRange));
	assert(zr.inside(fromZRange));
}

CutToEvaluator::~CutToEvaluator()
{
	param->DecRefCount();
}
	
bool CutToEvaluator::IsComplex()
{
	return param->IsComplex();
}

int CutToEvaluator::Dimensions()
{
	return param->Dimensions();
}

bool CutToEvaluator::GetDefaultResolution(vecN3& res)
{
	bool haveRes = param->GetDefaultResolution(res);
	if(!haveRes)
		return false;

	Range fromXRange, fromYRange, fromZRange;
	param->GetRanges(fromXRange,fromYRange,fromZRange);

	// #### Perhaps its inaccurate ?

	res.x = (int) (res.x * xRange.size() / fromXRange.size());
	res.y = (int) (res.y * yRange.size() / fromYRange.size());
	res.z = (int) (res.z * zRange.size() / fromZRange.size());

	return true;
}

void CutToEvaluator::EvaluateReal(vecR3 pos, int n, number values[])
{
	param->EvaluateReal(pos,n,values);
}

void CutToEvaluator::EvaluateComplex(vecR3 pos, int n, complex<number> values[])
{
	param->EvaluateComplex(pos,n,values);
}

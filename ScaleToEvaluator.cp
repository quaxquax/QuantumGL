#include "ScaleToEvaluator.h"
#include <assert.h>

ScaleToEvaluator::ScaleToEvaluator(ExpressionEvaluator *p, Range xr, Range yr, Range zr)
	: param(retain(p))
{
	assert(p);
	xRange = xr;
	yRange = yr;
	zRange = zr;
	p->GetRanges(fromXRange,fromYRange,fromZRange);
}

ScaleToEvaluator::~ScaleToEvaluator()
{
	param->DecRefCount();
}
	
bool ScaleToEvaluator::IsComplex()
{
	return param->IsComplex();
}

int ScaleToEvaluator::Dimensions()
{
	return param->Dimensions();
}

bool ScaleToEvaluator::GetDefaultResolution(vecN3& res)
{
	return param->GetDefaultResolution(res);
}

void ScaleToEvaluator::EvaluateReal(vecR3 pos, int n, number values[])
{
	pos.x = fromXRange.first
			+ (pos.x - xRange.first)
				/ (xRange.second-xRange.first)
				* (fromXRange.second-fromXRange.first);
	pos.y = fromYRange.first
			+ (pos.y - yRange.first)
				/ (yRange.second-yRange.first)
				* (fromYRange.second-fromYRange.first);
	pos.z = fromZRange.first
			+ (pos.z - zRange.first)
				/ (zRange.second-zRange.first)
				* (fromZRange.second-fromZRange.first);

	param->EvaluateReal(pos,n,values);
}

void ScaleToEvaluator::EvaluateComplex(vecR3 pos, int n, complex<number> values[])
{
	pos.x = fromXRange.first
			+ (pos.x - xRange.first)
				/ (xRange.second-xRange.first)
				* (fromXRange.second-fromXRange.first);
	pos.y = fromYRange.first
			+ (pos.y - yRange.first)
				/ (yRange.second-yRange.first)
				* (fromYRange.second-fromYRange.first);
	pos.z = fromZRange.first
			+ (pos.z - zRange.first)
				/ (zRange.second-zRange.first)
				* (fromZRange.second-fromZRange.first);

	param->EvaluateComplex(pos,n,values);
}

GeneralCubicData* ScaleToEvaluator::DoCreateField(vecN3 res, bool forceRes)
{
	return param->CreateField(res,forceRes);
}

#include "ZoomEvaluator.h"
#include "RealVariable.h"
#include <assert.h>

ZoomEvaluator::ZoomEvaluator(ExpressionEvaluator *p, RealVariable* f)
	: param(retain(p)), factor(retain(f))
{
	assert(p);
	assert(f);
	xRange = yRange = zRange = Range(-1,1); //###
}

ZoomEvaluator::~ZoomEvaluator()
{
	param->DecRefCount();
}
	
bool ZoomEvaluator::IsComplex()
{
	return param->IsComplex();
}

int ZoomEvaluator::Dimensions()
{
	return param->Dimensions();
}

bool ZoomEvaluator::GetDefaultResolution(vecN3& res)
{
	bool haveRes = param->GetDefaultResolution(res);
	if(!haveRes)
		return false;
		
	number f = 1/factor->value;
	if(f < 1)
	{
		res.x = (int) (res.x * f);
		res.y = (int) (res.y * f);
		res.z = (int) (res.z * f);
	}
	return true;
}

void ZoomEvaluator::EvaluateReal(vecR3 pos, int n, number values[])
{
	number f = 1/factor->value;
	pos.x *= (int) (pos.x * f);
	pos.y *= (int) (pos.y * f);
	pos.z *= (int) (pos.z * f);

	param->EvaluateReal(pos,n,values);
}

void ZoomEvaluator::EvaluateComplex(vecR3 pos, int n, complex<number> values[])
{
	number f = 1/factor->value;
	pos.x *= f;
	pos.y *= f;
	pos.z *= f;

	param->EvaluateComplex(pos,n,values);
}

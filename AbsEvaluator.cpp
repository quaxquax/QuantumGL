#include "AbsEvaluator.h"
#include <assert.h>
#include <stdio.h>

AbsEvaluator::AbsEvaluator(ExpressionEvaluator *p)
	: param(retain(p))
{
	assert(p);
	param->GetRanges(xRange,yRange,zRange);
}

AbsEvaluator::~AbsEvaluator()
{
	param->DecRefCount();
}
	
bool AbsEvaluator::IsComplex()
{
	return false;
}

int AbsEvaluator::Dimensions()
{
	return 1;
}

bool AbsEvaluator::GetDefaultResolution(vecN3& res)
{
	return param->GetDefaultResolution(res);
}

void AbsEvaluator::EvaluateReal(vecR3 pos, int n, number values[])
{
	if(n != 1)
		throw TypeMismatch();
	
	int nn = param->Dimensions();
	bool cc = param->IsComplex();
		
	if(cc)
	{
		complex<number> pValues[maxDimensions];
		param->EvaluateComplex(pos, nn, pValues);
	
		if(nn == 1)
			values[0] = abs(pValues[0]);
		else
		{
			number sum = 0;
			for(int i=0;i<nn;i++)
				sum += norm(pValues[i]);
			values[0] = sqrt(sum);
		}
	}
	else
	{
		number pValues[maxDimensions];
		param->EvaluateReal(pos, nn, pValues);
	
		if(nn == 1)
			values[0] = pValues[0] > 0 ? pValues[0] : -pValues[0];
		else
		{
			number sum = 0;
			for(int i=0;i<nn;i++)
				sum += pValues[i]*pValues[i];
			values[0] = sqrt(sum);
		}
	}
}

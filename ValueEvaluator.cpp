#include "ValueEvaluator.h"

bool ValueEvaluator::IsField()
{
	return false;
}

void ValueEvaluator::EvaluateReal(int n, number values[])
{
	throw TypeMismatch();
}

void ValueEvaluator::EvaluateComplex(int n, complex<number> values[])
{
	number tmp[maxDimensions];
	
	EvaluateReal(n,tmp);
	for(int i=0;i<n;i++)
		values[i] = tmp[i];
}


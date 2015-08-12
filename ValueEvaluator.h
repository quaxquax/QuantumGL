#ifndef _H_VALUE_EVAL
#define _H_VALUE_EVAL

#include "CommonEvaluator.h"

#include "QuantumMath.h"

class ValueEvaluator : public CommonEvaluator
{
public:
	virtual bool	IsField();

	virtual void 	EvaluateReal(int n, number values[]);
	virtual void 	EvaluateComplex(int n, complex<number> values[]);
};

#endif

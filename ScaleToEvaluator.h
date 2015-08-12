#ifndef _H_SCALETO_EVAL
#define _H_SCALETO_EVAL

#include "ExpressionEvaluator.h"

class ScaleToEvaluator : public ExpressionEvaluator
{
	Range			fromXRange, fromYRange, fromZRange;
	ExpressionEvaluator	*param;
public:
	ScaleToEvaluator(ExpressionEvaluator *param, Range xr, Range yr, Range zr);

	virtual bool	IsComplex();
	virtual int		Dimensions();
	virtual bool	GetDefaultResolution(vecN3& res);
	virtual void 	EvaluateReal(vecR3 pos, int n, number values[]);
	virtual void 	EvaluateComplex(vecR3 pos, int n, complex<number> values[]);
protected:
	virtual GeneralCubicData* 
					DoCreateField(vecN3 res = nullVecN3, bool forceRes = false);

	~ScaleToEvaluator();
};

#endif

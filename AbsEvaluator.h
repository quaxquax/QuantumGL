#include "ExpressionEvaluator.h"

class AbsEvaluator : public ExpressionEvaluator
{
	ExpressionEvaluator	*param;
public:
	AbsEvaluator(ExpressionEvaluator *p);
	
	virtual bool	IsComplex();
	virtual int		Dimensions();
	virtual bool	GetDefaultResolution(vecN3& res);
	virtual void 	EvaluateReal(vecR3 pos, int n, number values[]);
protected:
	~AbsEvaluator();
};

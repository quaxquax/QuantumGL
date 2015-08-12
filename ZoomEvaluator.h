#include "ExpressionEvaluator.h"
class RealVariable;

class ZoomEvaluator : public ExpressionEvaluator
{
	ExpressionEvaluator	*param;
	RealVariable *factor;
public:
	ZoomEvaluator(ExpressionEvaluator *p, RealVariable *f);
	
	virtual bool	IsComplex();
	virtual int		Dimensions();
	virtual bool	GetDefaultResolution(vecN3& res);
	virtual void 	EvaluateReal(vecR3 pos, int n, number values[]);
	virtual void 	EvaluateComplex(vecR3 pos, int n, complex<number> values[]);
protected:
	~ZoomEvaluator();
};

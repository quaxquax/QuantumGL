#ifndef _H_EXPRESSION_EVAL
#define _H_EXPRESSION_EVAL

#include "QuantumMath.h"
#include "CommonEvaluator.h"


class GeneralCubicData;
class DefinedField;

class ExpressionEvaluator : public CommonEvaluator
{
	bool			buffered;
	bool			notOwned;
	GeneralCubicData	*bufferedData;
	DefinedField	*theDefinedField;
	static vecN3	globalDefaultResolution;
	
			void	PreBuffer();
protected:
			Range	xRange, yRange, zRange;

	virtual bool	AlwaysCreateField();
	
	virtual GeneralCubicData* 
					DoCreateField(vecN3 res = nullVecN3, bool forceRes = false);

			vecN3	GetRequestedResolution(vecN3 res,bool forceRes);

			void	CheckTypeMismatch(bool complex, int n);

			vecR3	MapPos(vecR3 pos);
public:
					ExpressionEvaluator(GeneralCubicData* data = NULL);
					ExpressionEvaluator(DefinedField* defined);

	virtual bool	IsField() { return true; }
	virtual bool	IsComplex();
	virtual int		Dimensions();
	virtual bool	GetDefaultResolution(vecN3& res);
	
	virtual void 	EvaluateReal(vecR3 pos, int n, number values[]);
	virtual void 	EvaluateComplex(vecR3 pos, int n, complex<number> values[]);

	GeneralCubicData*
					CreateField(vecN3 res = nullVecN3, bool forceRes = false);
					
	static	void	SetGlobalDefaultResolution(vecN3 res);

			void	OrphanDefinedEvaluator();
			void	UpdateDefinedEvaluator(GeneralCubicData *data);

			void	GetRanges(Range& x, Range& y, Range& z);

protected:
	virtual			~ExpressionEvaluator();
};

#endif

#include "CalculationStage.h"
#include "QuantumMath.h"

#include <list>

class ExpressionEvaluator;
class GeneralCubicData;

class DefinedField : public CalculationStage
{
	ExpressionEvaluator	*expr;
	GeneralCubicData	*data;
	vecN3				res;
	bool				forceRes;

	list<ExpressionEvaluator*>	evaluators;
	
	RealVariable		*minEvaluator;
	RealVariable		*maxEvaluator;
protected:
	virtual void		CalculateStage();
public:
						DefinedField(string name, int res);
						~DefinedField();

			void		SetExpr(ExpressionEvaluator *expr);
						
	GeneralCubicData*	GetData();

			void		AddEvaluator(ExpressionEvaluator *e);
			void		RemoveEvaluator(ExpressionEvaluator *e);

			void		GetRanges(Range& x, Range& y, Range& z);
			
	RealVariable		*GetMinEvaluator();
	RealVariable		*GetMaxEvaluator();
};

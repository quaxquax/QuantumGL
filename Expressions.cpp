#include "Expressions.h"
#include "ValueExpressions.h"
#include "FieldExpressions.h"

CommonEvaluator* CreateBinaryOp(const BinValEvalFactory& operation, 
				CommonEvaluator *a, CommonEvaluator *b, bool allowMix)
{
	ValueEvaluator* avalue = dynamic_cast<ValueEvaluator*> (a);
	ValueEvaluator* bvalue = dynamic_cast<ValueEvaluator*> (b);
	ExpressionEvaluator* afield = dynamic_cast<ExpressionEvaluator*> (a);
	ExpressionEvaluator* bfield = dynamic_cast<ExpressionEvaluator*> (b);

	if(avalue && bvalue)
		return operation.Create(avalue,bvalue);
	else if(afield && bfield)
		return new FieldArithmeticEvaluator(afield,bfield,
								operation);
	else if(allowMix && afield && bvalue)
		return new FieldArithmeticEvaluator(afield,autorelease(new ConstantFieldEvaluator(bvalue)),
								operation);
	else if(allowMix && avalue && bfield)
		return new FieldArithmeticEvaluator(autorelease(new ConstantFieldEvaluator(avalue)),bfield,
								operation);
	else
	{
		assert(!allowMix);
		throw TypeMismatch();
	}
}
CommonEvaluator*	CreateNegateOp(CommonEvaluator *a)
{
	ValueEvaluator* avalue = dynamic_cast<ValueEvaluator*> (a);
	ExpressionEvaluator* afield = dynamic_cast<ExpressionEvaluator*> (a);
	
	assert(avalue != NULL || afield != NULL);
	assert(avalue == NULL || afield == NULL);
	
	if(avalue)
		return new NegateValueEvaluator(avalue);
	else
		return new NegateFieldEvaluator(afield);
}

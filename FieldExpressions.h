#include "ExpressionEvaluator.h"

class ValueEvaluator;
class BinaryValueEvaluator;
class BinValEvalFactory;
class FieldSampleValueEvaluator;

class ConstantFieldEvaluator : public ExpressionEvaluator
{
	ValueEvaluator *e;
public:
					ConstantFieldEvaluator(ValueEvaluator *e);
					~ConstantFieldEvaluator();

	virtual bool	IsComplex();
	virtual int		Dimensions();

	virtual void 	EvaluateReal(vecR3 pos, int n, number values[]);
	virtual void 	EvaluateComplex(vecR3 pos, int n, complex<number> values[]);
};

class FieldArithmeticEvaluator : public ExpressionEvaluator
{
	ExpressionEvaluator *a, *b;
	BinaryValueEvaluator *localEval;
	FieldSampleValueEvaluator
						*sampleA, *sampleB;
public:
					FieldArithmeticEvaluator(ExpressionEvaluator *a, ExpressionEvaluator *b,
											const BinValEvalFactory& factory);
					~FieldArithmeticEvaluator();

	virtual bool	IsComplex();
	virtual int		Dimensions();
	virtual bool	GetDefaultResolution(vecN3& res);

	virtual void 	EvaluateReal(vecR3 pos, int n, number values[]);
	virtual void 	EvaluateComplex(vecR3 pos, int n, complex<number> values[]);
};

class NegateFieldEvaluator : public ExpressionEvaluator
{
	ExpressionEvaluator *field;
public:
					NegateFieldEvaluator(ExpressionEvaluator *a);
					~NegateFieldEvaluator();

	virtual bool	IsComplex();
	virtual int		Dimensions();
	virtual bool	GetDefaultResolution(vecN3& res);

	virtual void 	EvaluateReal(vecR3 pos, int n, number values[]);
	virtual void 	EvaluateComplex(vecR3 pos, int n, complex<number> values[]);
};

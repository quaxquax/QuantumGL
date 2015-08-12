#include "ValueEvaluator.h"
#include <vector>
using std::vector;

class ExpressionEvaluator;

class ImaginaryUnitConstant : public ValueEvaluator
{
public:
	virtual bool	IsComplex();
	virtual int		Dimensions();
	virtual void 	EvaluateComplex(int n, complex<number> values[]);
};

class BinaryValueEvaluator : public ValueEvaluator
{
protected:
	ValueEvaluator	*valueA;
	ValueEvaluator	*valueB;
	int				kind;
	int				adim,bdim;
public:
	enum
	{
		kOneAndOne = 1 << 0,
		kOneAndMany = 1 << 1,
		kManyAndOne = 1 << 2,
		kManyAndMany = 1 << 3
	};

	BinaryValueEvaluator(ValueEvaluator *a,
						ValueEvaluator *b,
						int laws);
	~BinaryValueEvaluator();

	bool	IsComplex();
	int		Dimensions();
};

class BinValEvalFactory
{
public:
	virtual BinaryValueEvaluator*	Create(ValueEvaluator *a, ValueEvaluator *b) const = 0;
};

template <class BV>
class StdBinValEvalFactory : public BinValEvalFactory
{
public:
	virtual BinaryValueEvaluator* Create(ValueEvaluator *a, ValueEvaluator *b) const
		{
			return new BV(a,b);
		}
};

class AddValueEvaluator : public BinaryValueEvaluator
{
public:
					AddValueEvaluator(ValueEvaluator *a,
										ValueEvaluator *b);
	virtual void 	EvaluateReal(int n, number values[]);
	virtual void 	EvaluateComplex(int n, complex<number> values[]);
};

class SubtractValueEvaluator : public BinaryValueEvaluator
{
public:
					SubtractValueEvaluator(ValueEvaluator *a,
										ValueEvaluator *b);
	virtual void 	EvaluateReal(int n, number values[]);
	virtual void 	EvaluateComplex(int n, complex<number> values[]);
};


class MultiplyValueEvaluator : public BinaryValueEvaluator
{
public:
					MultiplyValueEvaluator(ValueEvaluator *a,
										ValueEvaluator *b);
	virtual void 	EvaluateReal(int n, number values[]);
	virtual void 	EvaluateComplex(int n, complex<number> values[]);
};

class DivideValueEvaluator : public BinaryValueEvaluator
{
public:
					DivideValueEvaluator(ValueEvaluator *a,
										ValueEvaluator *b);
	virtual void 	EvaluateReal(int n, number values[]);
	virtual void 	EvaluateComplex(int n, complex<number> values[]);
};

class DotValueEvaluator : public BinaryValueEvaluator
{
public:
					DotValueEvaluator(ValueEvaluator *a,
										ValueEvaluator *b);
	virtual void 	EvaluateReal(int n, number values[]);
	virtual void 	EvaluateComplex(int n, complex<number> values[]);

	int		Dimensions();
};

class NegateValueEvaluator : public ValueEvaluator
{
	ValueEvaluator	*value;
public:
					NegateValueEvaluator(ValueEvaluator *a);
					~NegateValueEvaluator();
					
	virtual void 	EvaluateReal(int n, number values[]);
	virtual void 	EvaluateComplex(int n, complex<number> values[]);

	virtual bool	IsComplex();
	virtual int		Dimensions();
};

class VectorValueEvaluator : public ValueEvaluator
{
	bool	iscomplex;
	vector<ValueEvaluator*>	params;
public:
					VectorValueEvaluator();
					~VectorValueEvaluator();
					
			void	AddComponent(ValueEvaluator *v);

	virtual void 	EvaluateReal(int n, number values[]);
	virtual void 	EvaluateComplex(int n, complex<number> values[]);

	virtual bool	IsComplex();
	virtual int		Dimensions();
};

class FieldSampleValueEvaluator : public ValueEvaluator
{
	ExpressionEvaluator *e;
	vecR3			at;
public:
					FieldSampleValueEvaluator(ExpressionEvaluator *e,vecR3 at);
					~FieldSampleValueEvaluator();
					
			void	SetSamplePos(vecR3 p);

	virtual void 	EvaluateReal(int n, number values[]);
	virtual void 	EvaluateComplex(int n, complex<number> values[]);

	virtual bool	IsComplex();
	virtual int		Dimensions();
};

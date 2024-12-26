#include "FieldExpressions.h"
#include "ValueExpressions.h"

ConstantFieldEvaluator::ConstantFieldEvaluator(ValueEvaluator *e)
	: e(retain(e))
{
}

ConstantFieldEvaluator::~ConstantFieldEvaluator()
{
	release(e);
}

bool ConstantFieldEvaluator::IsComplex()
{
	return e->IsComplex();
}

int ConstantFieldEvaluator::Dimensions()
{
	return e->Dimensions();
}

void ConstantFieldEvaluator::EvaluateReal(vecR3 pos, int n, number values[])
{
	e->EvaluateReal(n,values);
}

void ConstantFieldEvaluator::EvaluateComplex(vecR3 pos, int n, complex<number> values[])
{
	e->EvaluateComplex(n,values);
}

FieldArithmeticEvaluator::FieldArithmeticEvaluator(ExpressionEvaluator *a, ExpressionEvaluator *b,
											const BinValEvalFactory& factory)
	: a(retain(a)), b(retain(b))
{
	vecR3 pos0 = {0,0,0};
	sampleA = new FieldSampleValueEvaluator(a,pos0);
	sampleB = new FieldSampleValueEvaluator(b,pos0);
	localEval = factory.Create(sampleA, sampleB);
	
	Range ax, ay, az;
	Range bx, by, bz;
	
	a->GetRanges(ax,ay,az);
	b->GetRanges(bx,by,bz);
	
	xRange = intersectRanges(ax,bx);
	yRange = intersectRanges(ay,by);
	zRange = intersectRanges(az,bz);
	assert(xRange.size() > 0 && yRange.size() > 0 && zRange.size() > 0);
}

FieldArithmeticEvaluator::~FieldArithmeticEvaluator()
{
	release(a);
	release(b);
	release(sampleA);
	release(sampleB);
}

bool FieldArithmeticEvaluator::IsComplex()
{
	return localEval->IsComplex();
}

int FieldArithmeticEvaluator::Dimensions()
{
	return localEval->Dimensions();
}

void FieldArithmeticEvaluator::EvaluateReal(vecR3 pos, int n, number values[])
{
	sampleA->SetSamplePos(pos);
	sampleB->SetSamplePos(pos);
	localEval->EvaluateReal(n,values);
}

void FieldArithmeticEvaluator::EvaluateComplex(vecR3 pos, int n, complex<number> values[])
{
	sampleA->SetSamplePos(pos);
	sampleB->SetSamplePos(pos);
	localEval->EvaluateComplex(n,values);
}

bool FieldArithmeticEvaluator::GetDefaultResolution(vecN3& res)
{
	return a->GetDefaultResolution(res) || b->GetDefaultResolution(res);
}

NegateFieldEvaluator::NegateFieldEvaluator(ExpressionEvaluator *a)
	: field(retain(a))
{
	field->GetRanges(xRange,yRange,zRange);
}

NegateFieldEvaluator::~NegateFieldEvaluator()
{
	release(field);
}

bool NegateFieldEvaluator::IsComplex()
{
	return field->IsComplex();
}

int	 NegateFieldEvaluator::Dimensions()
{
	return field->Dimensions();
}

void NegateFieldEvaluator::EvaluateReal(vecR3 pos, int n, number values[])
{
	field->EvaluateReal(pos,n,values);
	for(int i=0;i<n;i++)
		values[i] = -values[i];
}

void NegateFieldEvaluator::EvaluateComplex(vecR3 pos, int n, complex<number> values[])
{
	field->EvaluateComplex(pos,n,values);
	for(int i=0;i<n;i++)
		values[i] = -values[i];
}


bool NegateFieldEvaluator::GetDefaultResolution(vecN3& res)
{
	return field->GetDefaultResolution(res);
}

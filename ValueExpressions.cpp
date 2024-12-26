#include "ValueExpressions.h"
#include "ExpressionEvaluator.h"
#include <algorithm>
using std::max;

bool ImaginaryUnitConstant::IsComplex()
{
	return true;
}

int ImaginaryUnitConstant::Dimensions()
{
	return 1;
}

void ImaginaryUnitConstant::EvaluateComplex(int n, complex<number> values[])
{
	if(n != 1)
		throw TypeMismatch();
	values[0] = complex<number>(0,1);
}

BinaryValueEvaluator::BinaryValueEvaluator(ValueEvaluator *a,
						ValueEvaluator *b,
						int laws)
	: valueA(retain(a)), valueB(retain(b))
{
	adim = a->Dimensions();
	bdim = b->Dimensions();
	kind = (adim == 1 ? 0 : 2) + (bdim == 1 ? 0 : 1);
	if(kind == 3 && adim != bdim)
		throw TypeMismatch();
	if((laws & (1 << kind)) == 0)
		throw TypeMismatch();
}

BinaryValueEvaluator::~BinaryValueEvaluator()
{
	valueA->DecRefCount();
	valueB->DecRefCount();
}

bool BinaryValueEvaluator::IsComplex()
{
	return valueA->IsComplex() || valueB->IsComplex();
}

int BinaryValueEvaluator::Dimensions()
{
	return max(adim,bdim);
}

AddValueEvaluator::AddValueEvaluator(ValueEvaluator *a,
									ValueEvaluator *b)
	: BinaryValueEvaluator(a,b,kOneAndOne | kManyAndMany)
{
}

void AddValueEvaluator::EvaluateReal(int n, number values[])
{
	if(n != adim || n != bdim)
		throw TypeMismatch();
	number tmpA[maxDimensions];
	number tmpB[maxDimensions];
	
	valueA->EvaluateReal(n,tmpA);
	valueB->EvaluateReal(n,tmpB);
	
	for(int i=0;i<n;i++)
		values[i] = tmpA[i] + tmpB[i];
}

void AddValueEvaluator::EvaluateComplex(int n, complex<number> values[])
{
	if(n != adim || n != bdim)
		throw TypeMismatch();
	complex<number> tmpA[maxDimensions];
	complex<number> tmpB[maxDimensions];
	
	valueA->EvaluateComplex(n,tmpA);
	valueB->EvaluateComplex(n,tmpB);
	
	for(int i=0;i<n;i++)
		values[i] = tmpA[i] + tmpB[i];
}

SubtractValueEvaluator::SubtractValueEvaluator(ValueEvaluator *a,
									ValueEvaluator *b)
	: BinaryValueEvaluator(a,b,kOneAndOne | kManyAndMany)
{
}

void SubtractValueEvaluator::EvaluateReal(int n, number values[])
{
	if(n != adim || n != bdim)
		throw TypeMismatch();
	number tmpA[maxDimensions];
	number tmpB[maxDimensions];
	
	valueA->EvaluateReal(n,tmpA);
	valueB->EvaluateReal(n,tmpB);
	
	for(int i=0;i<n;i++)
		values[i] = tmpA[i] - tmpB[i];
}

void SubtractValueEvaluator::EvaluateComplex(int n, complex<number> values[])
{
	if(n != adim || n != bdim)
		throw TypeMismatch();
	complex<number> tmpA[maxDimensions];
	complex<number> tmpB[maxDimensions];
	
	valueA->EvaluateComplex(n,tmpA);
	valueB->EvaluateComplex(n,tmpB);
	
	for(int i=0;i<n;i++)
		values[i] = tmpA[i] - tmpB[i];
}

MultiplyValueEvaluator::MultiplyValueEvaluator(ValueEvaluator *a,
									ValueEvaluator *b)
	: BinaryValueEvaluator(a,b,kOneAndOne | kOneAndMany | kManyAndOne)
{
}

void MultiplyValueEvaluator::EvaluateReal(int n, number values[])
{
	if(n == 1)
	{
		if(adim != 1 || bdim != 1)
			throw TypeMismatch();
		number tmpA, tmpB;
		
		valueA->EvaluateReal(1,&tmpA);
		valueB->EvaluateReal(1,&tmpB);
		
		values[0] = tmpA * tmpB;
	}
	else
	{
		if((adim == 1 && bdim == n) || (adim == n && bdim == 1))
			;
		else
			throw TypeMismatch();
			
		number tmpA;
		number tmpB[maxDimensions];
		
		if(adim == 1)
		{
			valueA->EvaluateReal(1,&tmpA);
			valueB->EvaluateReal(n,tmpB);
		}
		else
		{
			valueA->EvaluateReal(n,tmpB);
			valueB->EvaluateReal(1,&tmpA);
		}
		
		for(int i=0;i<n;i++)
			values[i] = tmpA * tmpB[i];
	}
}

void MultiplyValueEvaluator::EvaluateComplex(int n, complex<number> values[])
{
	if(n == 1)
	{
		if(adim != 1 || bdim != 1)
			throw TypeMismatch();
		complex<number> tmpA, tmpB;
		
		valueA->EvaluateComplex(1,&tmpA);
		valueB->EvaluateComplex(1,&tmpB);
		
		values[0] = tmpA * tmpB;
	}
	else
	{
		if((adim == 1 && bdim == n) || (adim == n && bdim == 1))
			;
		else
			throw TypeMismatch();
			
		complex<number> tmpA;
		complex<number> tmpB[maxDimensions];
		
		if(adim == 1)
		{
			valueA->EvaluateComplex(1,&tmpA);
			valueB->EvaluateComplex(n,tmpB);
		}
		else
		{
			valueA->EvaluateComplex(n,tmpB);
			valueB->EvaluateComplex(1,&tmpA);
		}
		
		for(int i=0;i<n;i++)
			values[i] = tmpA * tmpB[i];
	}
}

DivideValueEvaluator::DivideValueEvaluator(ValueEvaluator *a,
									ValueEvaluator *b)
	: BinaryValueEvaluator(a,b,kOneAndOne | kManyAndOne)
{
}

void DivideValueEvaluator::EvaluateReal(int n, number values[])
{
	if(n == 1)
	{
		if(adim != 1 || bdim != 1)
			throw TypeMismatch();
		number tmpA, tmpB;
		
		valueA->EvaluateReal(1,&tmpA);
		valueB->EvaluateReal(1,&tmpB);
		
		values[0] = tmpA / tmpB;
	}
	else
	{
		if(adim != n || bdim != 1)
			throw TypeMismatch();
			
		number tmpA[maxDimensions];
		number tmpB;
		
		valueA->EvaluateReal(n,tmpA);
		valueB->EvaluateReal(1,&tmpB);
		tmpB = 1/tmpB;
		
		for(int i=0;i<n;i++)
			values[i] = tmpA[i] * tmpB;
	}
}

void DivideValueEvaluator::EvaluateComplex(int n, complex<number> values[])
{
	if(n == 1)
	{
		if(adim != 1 || bdim != 1)
			throw TypeMismatch();
		complex<number> tmpA, tmpB;
		
		valueA->EvaluateComplex(1,&tmpA);
		valueB->EvaluateComplex(1,&tmpB);
		
		values[0] = tmpA / tmpB;
	}
	else
	{
		if(adim != n || bdim != 1)
			throw TypeMismatch();
			
		complex<number> tmpA[maxDimensions];
		complex<number> tmpB;
		
		valueA->EvaluateComplex(n,tmpA);
		valueB->EvaluateComplex(1,&tmpB);
		tmpB = complex<number>(1)/tmpB;
		
		for(int i=0;i<n;i++)
			values[i] = tmpA[i] * tmpB;
	}
}

DotValueEvaluator::DotValueEvaluator(ValueEvaluator *a,
									ValueEvaluator *b)
	: BinaryValueEvaluator(a,b,kOneAndOne | kManyAndMany)
{
}

int DotValueEvaluator::Dimensions()
{
	return 1;
}

void DotValueEvaluator::EvaluateReal(int n, number values[])
{
	if(n != 1)
		throw TypeMismatch();
		
	number tmpA[maxDimensions];
	number tmpB[maxDimensions];

	assert(adim == bdim);
	valueA->EvaluateReal(adim,tmpA);
	valueB->EvaluateReal(bdim,tmpB);
		
	number val = 0;
	for(int i=0;i<adim;i++)
		val += tmpA[i] * tmpB[i];
	values[0] = val;
}

void DotValueEvaluator::EvaluateComplex(int n, complex<number> values[])
{
	if(n != 1)
		throw TypeMismatch();
		
	complex<number> tmpA[maxDimensions];
	complex<number> tmpB[maxDimensions];

	assert(adim == bdim);
	valueA->EvaluateComplex(adim,tmpA);
	valueB->EvaluateComplex(bdim,tmpB);
		
	complex<number> val = 0;
	for(int i=0;i<adim;i++)
		val += conj(tmpA[i]) * tmpB[i];
	values[0] = val;
}

NegateValueEvaluator::NegateValueEvaluator(ValueEvaluator *a)
	: value(retain(a))
{
}

NegateValueEvaluator::~NegateValueEvaluator()
{
	release(value);
}
					
void NegateValueEvaluator::EvaluateReal(int n, number values[])
{
	value->EvaluateReal(n,values);
	for(int i=0;i<n;i++)
		values[i] = -values[i];
}

void NegateValueEvaluator::EvaluateComplex(int n, complex<number> values[])
{
	value->EvaluateComplex(n,values);
	for(int i=0;i<n;i++)
		values[i] = -values[i];
}

bool NegateValueEvaluator::IsComplex()
{
	return value->IsComplex();
}

int NegateValueEvaluator::Dimensions()
{
	return value->Dimensions();
}

VectorValueEvaluator::VectorValueEvaluator()
	: iscomplex(false)
{
}

VectorValueEvaluator::~VectorValueEvaluator()
{
	for(vector<ValueEvaluator*>::iterator p = params.begin();
		p != params.end(); ++p)
	{
		release(*p);
	}
}
					
void VectorValueEvaluator::AddComponent(ValueEvaluator *v)
{
	if(v->Dimensions() != 1)
		throw TypeMismatch();
	if(v->IsComplex())
		iscomplex = true;
	params.push_back(retain(v));
}

void VectorValueEvaluator::EvaluateReal(int n, number values[])
{
	if(n != (int)params.size())
		throw TypeMismatch();
	for(int i=0;i<n;i++)
		params[i]->EvaluateReal(1,&values[i]);
}

void VectorValueEvaluator::EvaluateComplex(int n, complex<number> values[])
{
	if(n != (int)params.size())
		throw TypeMismatch();
	for(int i=0;i<n;i++)
		params[i]->EvaluateComplex(1,&values[i]);
}

bool VectorValueEvaluator::IsComplex()
{
	return iscomplex;
}

int VectorValueEvaluator::Dimensions()
{
	return params.size();
}

FieldSampleValueEvaluator::FieldSampleValueEvaluator(ExpressionEvaluator *e,vecR3 at)
	: e(retain(e)), at(at)
{
}

FieldSampleValueEvaluator::~FieldSampleValueEvaluator()
{
	release(e);
}
					
void FieldSampleValueEvaluator::SetSamplePos(vecR3 p)
{
	at = p;
}

void FieldSampleValueEvaluator::EvaluateReal(int n, number values[])
{
	e->EvaluateReal(at,n,values);
}

void FieldSampleValueEvaluator::EvaluateComplex(int n, complex<number> values[])
{
	e->EvaluateComplex(at,n,values);
}

bool FieldSampleValueEvaluator::IsComplex()
{
	return e->IsComplex();
}

int	FieldSampleValueEvaluator::Dimensions()
{
	return e->Dimensions();
}

#include "DefinedField.h"
#include "ExpressionEvaluator.h"
#include "CubicData.h"
#include <assert.h>
#include <iostream>
using namespace std;

static RealVariable *NewAnonymousVar()
{
	RealVariable *var = new RealVariable();
	var->constant = false;
	var->name = "";
	var->theRange = fullRange;
	
	return var;
}

DefinedField::DefinedField(string name, int r)
	: expr(NULL)
{
	res.x = res.y = res.z = r;
	forceRes = (r != 0);
	data = NULL;
	SetStageName(name);
	minEvaluator = NewAnonymousVar();
	maxEvaluator = NewAnonymousVar();
}

void DefinedField::SetExpr(ExpressionEvaluator *expr)
{
	assert(this->expr == NULL);	// call only one
	this->expr = retain(expr);
}

DefinedField::~DefinedField()
{
	if(data)
		data->DecRefCount();
	if(expr) expr->DecRefCount();
	for(list<ExpressionEvaluator*>::iterator p = evaluators.begin();
			p != evaluators.end(); ++p)
	{
		(*p)->OrphanDefinedEvaluator();
	}
	release(minEvaluator);
	release(maxEvaluator);
}

GeneralCubicData* DefinedField::GetData()
{
	if(!data)
		CalculateIfNecessary();
	assert(data);
	return data;
}

void DefinedField::CalculateStage()
{
	GeneralCubicData *oldData = data;
	
		// release old data afterwards, AbortedException might happen
	data = expr->CreateField(res,forceRes);
	
	if(oldData)
		oldData->DecRefCount();
	
	for(list<ExpressionEvaluator*>::iterator p = evaluators.begin();
			p != evaluators.end(); ++p)
	{
		(*p)->UpdateDefinedEvaluator(data);
	}
	minEvaluator->ChangeTo(data->min);
	maxEvaluator->ChangeTo(data->max);
	cout << "Defined Field: [" << data->min << ".." << data->max << "]\n";
}

void DefinedField::AddEvaluator(ExpressionEvaluator *e)
{
	evaluators.push_back(e);
}

void DefinedField::RemoveEvaluator(ExpressionEvaluator *e)
{
	evaluators.remove(e);
}

void DefinedField::GetRanges(Range& x, Range& y, Range& z)
{
	assert(expr);
	expr->GetRanges(x,y,z);
}

RealVariable *DefinedField::GetMinEvaluator()
{
	return minEvaluator;
}
RealVariable *DefinedField::GetMaxEvaluator()
{
	return maxEvaluator;
}


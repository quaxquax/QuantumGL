#include "RealVariable.h"

void RealVariable::ChangeTo(number newValue)
{
	if(newValue == value)
		return;
	value = newValue;
	if(value > theRange.second)
		value = theRange.second;
	else if(value < theRange.first)
		value = theRange.first;
	
	list<RealVariableWatcher*>::iterator p = watchers.begin();
	
	while(p != watchers.end())
	{
		(*p)->VariableChanged(this);
		++p;
	}
}

bool RealVariable::IsComplex()
{
	return false;
}

int RealVariable::Dimensions()
{
	return 1;
}

void RealVariable::EvaluateReal(int n, number values[])
{
	if(n != 1)
		throw TypeMismatch();
	values[0] = value;
}

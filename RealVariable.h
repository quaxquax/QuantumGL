#ifndef _H_REALVARIABLE
#define _H_REALVARIABLE

#include "ValueEvaluator.h"

#include <string>
#include <list>

using std::string;
using std::list;

class RealVariableWatcher;

class RealVariable : public ValueEvaluator
{
public:
	bool			constant;
	string			name;
	Range			theRange;
	number			value;
	
	list<RealVariableWatcher*>	watchers;
	
	void			ChangeTo(number newValue);

	virtual bool	IsComplex();
	virtual int		Dimensions();
	virtual void	EvaluateReal(int n, number values[]);
};

class RealVariableWatcher
{
public:
	virtual void	VariableChanged(RealVariable* var) = 0;
	
	virtual ~RealVariableWatcher() {}
};

#endif

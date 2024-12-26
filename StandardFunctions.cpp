#include "StandardFunctions.h"
#include "ValueExpressions.h"
#include "QuantumDescription.h"

#include <cmath>
#include <complex>
#include <map>
#include <string>
using namespace std;


typedef number (*realFun)(number);
typedef complex<number> (*complexFun)(complex<number>);
typedef pair<realFun,complexFun> funPair;

map<string,funPair>		standardFunctions;

class StandardFunctionEvaluator : public ValueEvaluator
{
	ValueEvaluator *param;
	realFun			rfun;
	complexFun		cfun;
public:
					StandardFunctionEvaluator(funPair funs, ValueEvaluator *param);
					~StandardFunctionEvaluator();

	virtual void 	EvaluateReal(int n, number values[]);
	virtual void 	EvaluateComplex(int n, complex<number> values[]);

	virtual bool	IsComplex();
	virtual int		Dimensions();
};

StandardFunctionEvaluator::StandardFunctionEvaluator(funPair funs, ValueEvaluator *param)
	: param(retain(param)), rfun(funs.first), cfun(funs.second)
{
	if(param->Dimensions() != 1)
		throw TypeMismatch();
	if(param->IsComplex() && !cfun)
		throw TypeMismatch();
}

StandardFunctionEvaluator::~StandardFunctionEvaluator()
{
	release(param);
}

void StandardFunctionEvaluator::EvaluateReal(int n, number values[])
{
	if(!rfun)
		ValueEvaluator::EvaluateReal(n,values);
	else
	{
		if(n != 1)
			throw TypeMismatch();
		number tmp;
		param->EvaluateReal(1,&tmp);
		values[0] = (*rfun)(tmp);
	}
}

void StandardFunctionEvaluator::EvaluateComplex(int n, complex<number> values[])
{
	if(!cfun)
		ValueEvaluator::EvaluateComplex(n,values);
	else
	{
		if(n != 1)
			throw TypeMismatch();
		complex<number> tmp;
		param->EvaluateComplex(1,&tmp);
		values[0] = (*cfun)(tmp);
	}
}

bool StandardFunctionEvaluator::IsComplex()
{
	return (!rfun) || param->IsComplex();
}

int StandardFunctionEvaluator::Dimensions()
{
	return 1;
}


ValueEvaluator*	CreateStandardFunctionEvaluator(const string& f, ValueEvaluator *param)
{
	map<string,funPair>::iterator p = standardFunctions.find(f);
	assert(p != standardFunctions.end());
	
	return new StandardFunctionEvaluator(p->second,param);
}

#define WRAP_REAL(f) static number std_ ## f (number x) { return f(x); }
#define WRAP_COMP(f) static complex<number> std_ ## f (complex<number> x) { return f(x); }
#define WRAP_FUN(f)	WRAP_REAL(f) WRAP_COMP(f)

#define REG_FUN(f)	standardFunctions[#f] = funPair((realFun)&std_ ## f,(complexFun)&std_ ## f);
#define REG_REAL(f)	standardFunctions[#f] = funPair((realFun)&std_ ## f,NULL);

WRAP_FUN(exp)
WRAP_FUN(sin)
WRAP_FUN(cos)
WRAP_REAL(tan)	// g++
static complex<number> std_tan(complex<number> x) { return sin(x) / cos(x); }
WRAP_FUN(log)
WRAP_REAL(log10)	// g++
static complex<number> std_log10(complex<number> x) { return log(x) / complex<number>(log(10.)); }

WRAP_FUN(sqrt)
WRAP_REAL(asin)	//!
WRAP_REAL(acos)	//!
WRAP_REAL(atan)	//!
WRAP_FUN(sinh)
WRAP_FUN(cosh)
WRAP_REAL(tanh)	// g++
static complex<number> std_tanh(complex<number> x) { return sinh(x) / cosh(x); }

void RegisterStandardFunctions()
{
	REG_FUN(exp)
	REG_FUN(sin)
	REG_FUN(cos)
	REG_FUN(tan)
	REG_FUN(log)
	REG_FUN(log10)
	REG_FUN(sqrt)
	REG_REAL(asin)
	REG_REAL(acos)
	REG_REAL(atan)
	REG_FUN(sinh)
	REG_FUN(cosh)
	REG_FUN(tanh)

	for(map<string,funPair>::iterator p = standardFunctions.begin();
		p != standardFunctions.end(); ++p)
	{
		RegisterStandardFunctionID(p->first);
	}
}


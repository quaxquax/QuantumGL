#include "FieldFunctionImpl.h"

#include "QuantumDescription.h"

#include <iostream>
using std::cout;
using std::endl;

vector<AdaptedEvaluatorFactory*> registeredFieldFunctions;

AdaptedEvaluatorFactory::AdaptedEvaluatorFactory
					(const string& theName, int theN)
	: name(theName), n(theN)
{
}

bool AdaptedEvaluatorFactory::CheckArgs(int theN)
{
	return theN == n; 
}

ExpressionEvaluator * FindFieldFunction(
			const string&	name,
			const vector<CommonEvaluator*>& fieldArgs)
{
	int n = registeredFieldFunctions.size();
	
	for(int i=0;i<n;i++)
	{
		AdaptedEvaluatorFactory *factory = registeredFieldFunctions[i];
		
		if(factory->GetName() == name)
		{
			if(!factory->CheckArgs(fieldArgs.size()))
			{
				cout << fieldArgs.size() << " arguments." << endl;
					
				string msg = "Parameter List Mismatch (function " + name +")\n";
				throw DescError(msg);
			}
			return factory->MakeEvaluator(fieldArgs);
		}
	}
	throw DescError("Undefined Field Function???");
	return NULL;
}

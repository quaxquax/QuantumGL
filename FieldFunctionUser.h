#ifndef _H_FIELD_FUNCTION_USER
#define _H_FIELD_FUNCTION_USER

#include <vector>
#include <string>
using std::vector;
using std::string;

class CommonEvaluator;

ExpressionEvaluator * FindFieldFunction(
			const string &	name,
			const vector<CommonEvaluator*>& fieldArgs);

void RegisterFieldFunctions();
			
#endif

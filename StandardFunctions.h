#include <string>

class ValueEvaluator;

void RegisterStandardFunctions();

ValueEvaluator*	CreateStandardFunctionEvaluator(const std::string& f, ValueEvaluator *param); 

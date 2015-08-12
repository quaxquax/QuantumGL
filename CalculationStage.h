#ifndef _CALCULATIONSTAGE_H_
#define _CALCULATIONSTAGE_H_

#include <vector>
using std::vector;

#include "RealVariable.h"

class CalculationStage : private RealVariableWatcher
{
	string			stageName;
	
	unsigned		cachedID;
	unsigned		currentID;
	static unsigned	nextID;
	
	vector<CalculationStage*> dependsOn;
	
	void 		VariableChanged(RealVariable* var);
protected:
	virtual void	CalculateStage() = 0;
public:
				CalculationStage();

	void		DependOn(CalculationStage* prevStage);
	void		DependOn(RealVariable* var);
	void		MarkForRecalculation();
	void		CalculateIfNecessary();
	
	static void	AllUpdated();
	
	void		SetStageName(string s);
};

#endif

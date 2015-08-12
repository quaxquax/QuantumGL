#include "CalculationStage.h"
#include "QuantumProgress.h"

unsigned CalculationStage::nextID = 1;

CalculationStage::CalculationStage()
	: cachedID(0), currentID(1)
{
	stageName = "";
}

void CalculationStage::DependOn(CalculationStage* prevStage)
{
	dependsOn.push_back(prevStage);
}
void CalculationStage::DependOn(RealVariable* var)
{
	var->watchers.push_back(this);
}

void CalculationStage::MarkForRecalculation()
{
	currentID = nextID;
}

void CalculationStage::CalculateIfNecessary()
{
	vector<CalculationStage*>::iterator p = dependsOn.begin();
	
	while(p != dependsOn.end())
	{
		(*p)->CalculateIfNecessary();
		if((*p)->currentID > currentID)
			currentID = (*p)->currentID;
		++p;
	}
	
	CheckAbort();
	if(currentID > cachedID)
	{
		SetProgressMessage(stageName);
		CalculateStage();
		CheckAbort();
		cachedID = currentID;
	}
}

void CalculationStage::AllUpdated()
{
	nextID++;
}

void CalculationStage::VariableChanged(RealVariable* /*var*/)
{
	MarkForRecalculation();
}

void CalculationStage::SetStageName(string s)
{
	stageName = s;
}

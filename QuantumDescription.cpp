#include "QuantumDescription.h"
#include "quantum.tab.h"
#include "RealVariable.h"
#include "Animator.h"
#include "DefinedField.h"
#include "VisualObject.h"
#include <iostream>
#include <functional>
#include <algorithm>

ParsePosition curPosition;

definedFieldsMap definedFields;
definedRealVariablesMap definedRealVariables;
theVisualObjectsList theVisualObjects;
//realVariablesList unnamedVariables;
identifierTypeMap definedIdentifierTypes;
theAnimationsList theAnimations;
SceneOptions theSceneOptions;

void RegisterIdentifierAs(string s, int type)
{
	const pair<const std::string, int> p(s,type);
	definedIdentifierTypes.insert(p);
}

void RegisterFieldFunctionID(string s)
{
	RegisterIdentifierAs(s,kTokFieldFunctionIdentifier);
}

void RegisterStandardFunctionID(string s)
{
	RegisterIdentifierAs(s,kTokStandardFunctionIdentifier);	
}

void DefineRealVariable(RealVariable *var)
{
	const pair<const std::string, RealVariable*> p(var->name,var);
	definedRealVariables.insert(p);
	RegisterIdentifierAs(var->name,kTokRealVariableIdentifier);
}

SceneOptions::SceneOptions()
{
	boxType = kBoxWhite;
	boxDivisions = 0;
	xr = yr = zr = fullRange;
	fogRange = fullRange;
	backgroundColor.x = backgroundColor.y = backgroundColor.z = 0;
}

void ReleaseDescription()
{
	for(definedFieldsMap::iterator p = definedFields.begin();
		p != definedFields.end();++p)
	{
		delete p->second;
	}
	definedFields.clear();
	for(definedRealVariablesMap::iterator p = definedRealVariables.begin();
		p != definedRealVariables.end();++p)
	{
		p->second->DecRefCount();
	}
	definedRealVariables.clear();
	definedIdentifierTypes.clear();
	for(theVisualObjectsList::iterator p = theVisualObjects.begin();
		p != theVisualObjects.end();++p)
	{
		delete *p;
	}
	theVisualObjects.clear();
	for(theAnimationsList::iterator p = theAnimations.begin();
		p != theAnimations.end();++p)
	{
		delete *p;
	}
	theAnimations.clear();
}

#ifndef _H_QUANTUM_DESCRIPTION
#define _H_QUANTUM_DESCRIPTION

#include "QuantumMath.h"

#include <string>
#include <map>
#include <list>
#include <exception>


using std::string;
using std::map;
using std::list;

class CommonEvaluator;
class ExpressionEvaluator;
class ValueEvaluator;
class VisualObject;
class GeneralCubicData;
class RealVariable;
class Animator;
class DefinedField;
class PointList;

struct yyValueType
{
	number	realNum;
	string	str;

	CommonEvaluator*	expr;
	ExpressionEvaluator*	field;
	RealVariable*	var;
	ValueEvaluator* value;
	Range	range;
	int		integer;
	PointList*	points;
	vecR3	real3;
};
#define YYSTYPE yyValueType

struct ParsePosition
{
	int		line;
};

extern ParsePosition curPosition;

class DescError : public std::exception
{
public:
	string	message;
	ParsePosition pos;
	
			DescError(string msg)
				 : message(msg), pos(curPosition) {}
			~DescError() throw() {}
	
    virtual const char* what () const throw() {return message.c_str();}
};

class RangeMismatch : public DescError
{
public:
			RangeMismatch(Range /*x1*/,Range /*y1*/,Range /*z1*/,
						Range /*x2*/,Range /*y2*/,Range /*z2*/)
				: DescError("Range Mismatch") {}
};

enum
{
	kBoxNone = 0,
	kBoxWhite = 1,
	kBoxColored = 2,
	kBoxOneCustomColor = 3,
	kBoxThreeCustomColors = 4
};

struct SceneOptions
{
	int		boxType;
	int		boxDivisions;
	Range	xr,yr,zr;
	Range	fogRange;
	vecR3	backgroundColor;
	vecR3	boxColorX;
	vecR3	boxColorY;
	vecR3	boxColorZ;
			SceneOptions();
};

typedef map<string, DefinedField*> definedFieldsMap;
typedef map<string, RealVariable*> definedRealVariablesMap;
typedef map<string, int> identifierTypeMap;
typedef list<VisualObject*> theVisualObjectsList;
//typedef list<RealVariable*> realVariablesList;
typedef list<Animator*> theAnimationsList;

extern definedFieldsMap definedFields;
extern definedRealVariablesMap definedRealVariables;
extern identifierTypeMap definedIdentifierTypes;
extern theVisualObjectsList theVisualObjects;
//extern realVariablesList unnamedVariables;
extern theAnimationsList theAnimations;
extern SceneOptions theSceneOptions;

void RegisterIdentifierAs(string s, int type);
void RegisterFieldFunctionID(string s);
void RegisterStandardFunctionID(string s);
void DefineRealVariable(RealVariable *var);

	// in QuantumGL.cp:
void ResizeDisplayTo(int w, int h);

void ReleaseDescription();

#endif


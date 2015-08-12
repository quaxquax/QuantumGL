%{
#include "QuantumMath.h"
#include "QuantumDescription.h"

#include "BSPTree.h"

#include "ReadEvaluator.h"
#include "AbsEvaluator.h"
#include "ZoomEvaluator.h"
#include "ScaleToEvaluator.h"
#include "CutToEvaluator.h"

#include "IsoSurface.h"
#include "Slice.h"
#include "Vectors.h"
#include "Flow.h"

#include "ExplicitPointList.h"
#include "GridPointList.h"

#include "RealVariable.h"

#include "FieldFunctionUser.h"

#include "Animator.h"

#include "DefinedField.h"

#include "ValueExpressions.h"
#include "FieldExpressions.h"
#include "Expressions.h"
#include "StandardFunctions.h"

#include <vector>
#include <stack>
#include <algorithm>
using std::vector;
using std::stack;
using std::for_each;

#include <iostream>

#include <time.h>

void yyerror(const char *errstr);
int yylex();
int yyparse();
#include <stdio.h>

void yyerror(const char *errstr)
{
	//printf("%s\n",errstr);
	throw DescError(errstr);
}

static VisualObject* currentObject;
static IsoSurface* currentSurface;
static Slice* currentSlice;
static Vectors* currentVectors;
static Flow* currentFlow;
static DefinedField* currentDefinedField;

static stack<CalculationStage*> calcStages;

static DefinedField* GetFieldVar(const string& name)
{
	definedFieldsMap::iterator p;
	p = definedFields.find(name);
	if(p == definedFields.end())
		throw DescError(string("undefined identifier: ") + name);
	return p->second;
}

static RealVariable* GetRealVar(const string& name)
{
	definedRealVariablesMap::iterator p;
	p = definedRealVariables.find(name);
	if(p == definedRealVariables.end())
		throw DescError(string("undefined identifier: ") + name);
	return p->second;
}

static void DependOnVar(RealVariable* var)
{
	if(var->constant)
		return;
	if(!calcStages.empty())
		calcStages.top()->DependOn(var);
	else
		throw DescError(string("must use constants!!!"));
}

static string lastStatusMsg;
static clock_t statusStart;

static void WriteStatus(const string& msg)
{
	std::cout << "[line " << curPosition.line << "]\t" << msg << "..." << endl;
	lastStatusMsg = msg;
	statusStart = clock();
}

static void WriteDone()
{
	clock_t t = clock() - statusStart;
	std::cout << "[line " << curPosition.line << "]\t"
			 << lastStatusMsg << " done in " << double(t)/CLOCKS_PER_SEC << endl;
}

static stack<vector<CommonEvaluator*> >		funcParams;

static AnimationSection	animSection;
static Animator	anim;

%}

%token					kTok_variable
%token					kTok_field
%token					kTok_purge
%token					kTok_isosurface
%token					kTok_slice
%token					kTok_vectors
%token					kTok_flow
%token					kTok_window
%token					kTok_nobox
%token					kTok_colorbox
%token					kTok_grid
%token					kTok_world_size
%token					kTok_fog
%token					kTok_background

%token					kTok_min
%token					kTok_max
%token					kTok_cx
%token					kTok_cy
%token					kTok_cz

%token					kTok_resolution
%token					kTok_at
%token					kTok_cyclic
%token					kTok_color
%token					kTok_cutout
%token					kTok_transparency
%token					kTok_framed
%token					kTok_shininess
%token					kTok_secondary_color
%token					kTok_sphere_color
%token					kTok_sphere_radius
%token					kTok_length
%token					kTok_stepsize

%token					kTok_x
%token					kTok_y
%token					kTok_z

%token					kTok_i

%token					kTok_read
%token					kTok_abs
%token					kTok_zoom

%token					kTok_scaled_to
%token					kTok_cut_to
%token					kTok_filled_to
%token					kTok_with

%token					kTokAssign

%token	<str>			kTokIdentifier
%token	<realNum>		kTokReal
%token	<integer>		kTokPosInteger
%token	<str>			kTokString

%token	<str>			kTokFieldIdentifier
%token	<str>			kTokRealVariableIdentifier
%token	<str>			kTokFieldFunctionIdentifier
%token	<str>			kTokStandardFunctionIdentifier

%token					kTok_animate
%token					kTok_from
%token					kTok_to
%token					kTok_and
%token					kTok_in
%token					kTok_frames
%token					kTok_frame
%token					kTok_inclusive

%type	<value>			value_expression
%type	<field>			field_expression

%type	<expr>			expression arithmetic term primary

%type	<realNum>		real_constant basic_real_constant
%type	<range>			range_specifier
%type	<range>			optional_range_specifier
%type	<range>			range_or_diameter

%type	<integer>		xyz

%type	<integer>		positive_integer_constant

%type	<integer>		sequence_dur

%type	<points>		point_list
%type	<points>		explicit_point_list

%type	<real3>			constant_color

%%

input:			
				{	// initialisation;
					currentObject = NULL;
					currentSurface = NULL;
					currentSlice = NULL;
					currentVectors = NULL;
					currentFlow = NULL;
					currentDefinedField = NULL;
					while(!calcStages.empty())
					{
						std::cerr << "###unclean stack!\n";
						calcStages.pop();
					}
					
					theSceneOptions = SceneOptions();
				}
			|	input statement
			;

statement:		variable_decl
			|	field_decl
			|	isosurface
			|	slice
			|	vectors
			|	flow
			|	animation
			|	assignment
			/*|	window_directive*/
			|	box_directive
			|	resolution_directive
			|	fog_directive
			|	background_directive
			;

variable_decl:	kTok_variable kTokIdentifier 
				optional_range_specifier kTokAssign real_constant ';'
				{
					RealVariable *var = new RealVariable;
					var->name = $2;
					var->constant = false;
					var->theRange = $3;
					var->value = $5;
					DefineRealVariable(var);
				}
			;
			
range_specifier:
				'[' real_constant ',' real_constant ']'
				{
					$$ = Range($2,$4);
				}
			;

			
optional_range_specifier:
				{
					$$ = fullRange;
				}
			|	range_specifier
			;
			
range_or_diameter:
				range_specifier
			|	real_constant
				{
					$$ = Range(- $1 / 2, $1 / 2);
				}
			;

real_constant:	basic_real_constant
			|	'-' basic_real_constant
				{
					$$ = -$2;
				}
			;
			
basic_real_constant:	kTokReal
			|	positive_integer_constant
				{
					$$ = (number) $1;
				}
			/*|	kTokFieldIdentifier '.' kTok_min
				{
					//assert(false);	// ### no longer constant
					$$ = GetFieldVar($1)->GetData()->min;
				}
			|	kTokFieldIdentifier '.' kTok_max
				{
					//assert(false);	// ### no longer constant
					$$ = GetFieldVar($1)->GetData()->max;
				}*/
			;

positive_integer_constant:
				kTokPosInteger
			/*|	kTokFieldIdentifier '.' kTok_cx
				{	// ### is it necessary to calculate field here???
					$$ = GetFieldVar($1)->GetData()->cx;
				}
			|	kTokFieldIdentifier '.' kTok_cy
				{
					$$ = GetFieldVar($1)->GetData()->cy;
				}
			|	kTokFieldIdentifier '.' kTok_cz
				{
					$$ = GetFieldVar($1)->GetData()->cz;
				}*/
			;


value_expression:
				expression
				{
					ValueEvaluator *expr = dynamic_cast<ValueEvaluator*> ($1);
					assert(expr); //###
					$$ = expr;
				}
			;

field_expression:
				expression
				{
					if(ExpressionEvaluator *expr = dynamic_cast<ExpressionEvaluator*> ($1))
						$$ = expr;
					else
					{
						ValueEvaluator *val = dynamic_cast<ValueEvaluator*> ($1);
						assert(val);
						$$ = autorelease(new ConstantFieldEvaluator(val));
					}
				}
			;
			
expression:		arithmetic
			|	field_expression kTok_scaled_to range_or_diameter
				{
					$$ = autorelease(new ScaleToEvaluator($1,$3,$3,$3));
				}
			|	field_expression kTok_scaled_to 
					'<' range_or_diameter ',' range_or_diameter ',' range_or_diameter '>'
				{
					$$ = autorelease(new ScaleToEvaluator($1,$4,$6,$8));
				}
			|	field_expression kTok_cut_to range_or_diameter
				{
					$$ = autorelease(new CutToEvaluator($1,$3,$3,$3));
				}
			|	field_expression kTok_cut_to 
					'<' range_or_diameter ',' range_or_diameter ',' range_or_diameter '>'
				{
					$$ = autorelease(new CutToEvaluator($1,$4,$6,$8));
				}
			;
			
arithmetic:		term
			|	expression '+' term
				{
					$$ = autorelease(CreateBinaryOp(StdBinValEvalFactory<AddValueEvaluator>(),$1,$3,false));
				}
			|	expression '-' term
				{
					$$ = autorelease(CreateBinaryOp(StdBinValEvalFactory<SubtractValueEvaluator>(),$1,$3,false));
				}
			|	'-' term
				{
					$$ = autorelease(CreateNegateOp($2));
				}
			;
			
term:			primary
			|	term '*' primary
				{
					$$ = autorelease(CreateBinaryOp(StdBinValEvalFactory<MultiplyValueEvaluator>(),$1,$3,true));
				}
			|	term '/' primary
				{
					$$ = autorelease(CreateBinaryOp(StdBinValEvalFactory<DivideValueEvaluator>(),$1,$3,true));
				}
			|	term '.' primary
				{
					$$ = autorelease(CreateBinaryOp(StdBinValEvalFactory<DotValueEvaluator>(),$1,$3,true));
				}
			;
					
primary:		'(' expression ')'
				{ $$ = $2; }		
		
			|	kTokRealVariableIdentifier
				{
					RealVariable *var = GetRealVar($1);
					DependOnVar(var);
					$$ = var;
				}
			|	basic_real_constant
				{
					RealVariable *var = new RealVariable;
					var->name = "";
					var->constant = true;
					var->value = $1;
					var->theRange = Range($1,$1);
					//unnamedVariables.push_back(var);
					$$ = autorelease(var);
				}
				
			|	kTok_i
				{
					$$ = autorelease(new ImaginaryUnitConstant());
				}
			|	'<'
				{
					funcParams.push(vector<CommonEvaluator*>());
				}
				ff_field_args1 '>'
				{
					VectorValueEvaluator *e = new VectorValueEvaluator();
					vector<CommonEvaluator*>& v = funcParams.top();
					for(vector<CommonEvaluator*>::iterator p = v.begin();
						p != v.end(); ++p)
					{
						if(ValueEvaluator *val = dynamic_cast<ValueEvaluator*> (*p))
							e->AddComponent(val);
						else
							throw TypeMismatch();
					}
					funcParams.pop();
					$$ = autorelease(e);
				}
			|	kTokStandardFunctionIdentifier '(' expression ')'
				{
					if(ValueEvaluator *val = dynamic_cast<ValueEvaluator*> ($3))
						$$ = autorelease(CreateStandardFunctionEvaluator($1,val));
					else
					{
						ExpressionEvaluator *expr = dynamic_cast<ExpressionEvaluator*> ($3);
						assert(expr);

						assert(false);
					}
				}
			|	kTok_field '(' expression ')'
				{
					if(ValueEvaluator *val = dynamic_cast<ValueEvaluator*> ($3))
					{
						$$ = autorelease(new ConstantFieldEvaluator(val));
					}
					else
						throw TypeMismatch(); //### $$ = $3; ?
				}
			|	kTok_read '(' kTokString ')'
				{
					$$ = autorelease(new ReadEvaluator($3.c_str()));
				}
			|	kTok_abs '(' field_expression ')'
				{
					$$ = autorelease(new AbsEvaluator($3));
				}
			/*|	kTok_zoom '(' field_expression ',' real_var ')'
				{
					$$ = autorelease(new ZoomEvaluator($3,$5));
					DependOnVar($5);
				}*/
			|	kTokFieldIdentifier '.' kTok_min
				{
					DefinedField *defined = GetFieldVar($1);
					RealVariable *val = defined->GetMinEvaluator();
					assert(!calcStages.empty());
					calcStages.top()->DependOn(val);
					$$ = val;
				}
			|	kTokFieldIdentifier '.' kTok_max
				{
					DefinedField *defined = GetFieldVar($1);
					RealVariable *val = defined->GetMaxEvaluator();
					assert(!calcStages.empty());
					calcStages.top()->DependOn(val);
					$$ = val;
				}
			|	kTokFieldIdentifier
				{
					DefinedField *defined = GetFieldVar($1);
					assert(!calcStages.empty());
					calcStages.top()->DependOn(defined);
					$$ = autorelease(new ExpressionEvaluator(defined));
				}
			|	kTokFieldFunctionIdentifier
				{
					funcParams.push(vector<CommonEvaluator*>());
				}
				'(' ff_field_args ')'
				{
					$$ = autorelease(FindFieldFunction($1,funcParams.top()));
					funcParams.pop();
				}
			;
			
ff_field_args:	
			|	ff_field_args1
			;

ff_field_args1:	ff_field_arg
			|	ff_field_args1 ',' ff_field_arg
			;
			
ff_field_arg:	expression
				{
					funcParams.top().push_back($1);
				}
			;

field_decl:		kTok_field kTokIdentifier
				kTokAssign 
				{
					currentDefinedField = new DefinedField($2,0);
					calcStages.push(currentDefinedField);
				}
				field_expression ';'
				{
					WriteStatus("field " + $2);
					currentDefinedField->SetExpr($5);
					definedFields[$2] = currentDefinedField;
					currentDefinedField = NULL;
					calcStages.pop();
					RegisterIdentifierAs($2,kTokFieldIdentifier);
					WriteDone();
				}
			|	kTok_field kTokIdentifier kTok_resolution positive_integer_constant
				kTokAssign 
				{
					currentDefinedField = new DefinedField($2,$4);
					calcStages.push(currentDefinedField);
				}
				field_expression ';'
				{
					WriteStatus("field " + $2);
					currentDefinedField->SetExpr($7);
					definedFields[$2] = currentDefinedField;
					currentDefinedField = NULL;
					calcStages.pop();
					RegisterIdentifierAs($2,kTokFieldIdentifier);
					WriteDone();
				}
			;
			
isosurface:		kTok_isosurface
				{
					WriteStatus("isosurface");
					currentSurface = new IsoSurface();
					theVisualObjects.push_back(currentSurface);
					currentObject = currentSurface;
					calcStages.push(currentSurface);
					
					calcStages.push(currentSurface->GetFieldEvaluationStage());
				}
				field_expression 
				{
					calcStages.pop();
					
					currentSurface->SetData($3);
					
					Range xr,yr,zr;
					$3->GetRanges(xr,yr,zr);
					cout << "([" << xr.first << "," << xr.second << "], ["
						<< yr.first << "," << yr.second << "], ["
						<< zr.first << "," << zr.second << "])\n";
				}
				isosurface_options ';'
				{
					//#?#currentSurface->Build();
					calcStages.pop();
					currentSurface = NULL;
					currentObject = NULL;
					WriteDone();
				}
			;

isosurface_options:
			|	isosurface_options isosurface_option
			;

isosurface_option:
				object_option
			|	kTok_at value_expression
				{
					currentSurface->SetThreshold($2);
				}
			|	kTok_cyclic real_constant
				{
					currentSurface->SetCyclic($2);
				}
			;

slice:			kTok_slice
				{
					WriteStatus("slice");
					currentSlice = new Slice();
					theVisualObjects.push_back(currentSlice);
					currentObject = currentSlice;
					calcStages.push(currentSlice);
				}
				slice_options ';'
				{
					currentSlice->Build();
					currentSlice = NULL;
					currentObject = NULL;
					calcStages.pop();
					WriteDone();
				}
			;

slice_options:
			|	slice_options slice_option
			;

slice_option:	object_option
			|	xyz '=' value_expression
				{
					/*if($3->theRange.first < -1 || $3->theRange.second > 1
					 ||$3->value < -1 || $3->value > 1)
						throw DescError("Slice coordinate variable must be in [-1,1]");*/
					currentSlice->SetCoordValue($1,$3);
					//$3->watchers.push_back(currentSlice->GetCoordValueWatcher());
				}
			|	kTok_framed
				{
					currentSlice->SetFramed(true);
				}
			;

xyz:			kTok_x
				{ $$ = 0; }
			|	kTok_y
				{ $$ = 1; }
			|	kTok_z
				{ $$ = 2; }
			;

vectors:		kTok_vectors
				{
					WriteStatus("vectors");
					currentVectors = new Vectors();
					theVisualObjects.push_back(currentVectors);
					currentObject = currentVectors;
					calcStages.push(currentVectors);
				}
				field_expression 
				{
					currentVectors->SetData($3);
				}
				vectors_options ';'
				{
					calcStages.pop();
					currentVectors = NULL;
					currentObject = NULL;
					WriteDone();
				}
			;
vectors_options:
			|	vectors_options vectors_option
			;

vectors_option:	object_option
			|	kTok_secondary_color field_expression
				{
					currentVectors->SetSecondaryColor($2);
				}
			|	kTok_sphere_color field_expression
				{
					currentVectors->SetSphereColor($2);
				}
			|	kTok_sphere_radius field_expression
				{
					currentVectors->SetSphereSize($2);
				}
			|	kTok_at point_list
				{
					currentVectors->SetPoints($2);
				}
			;

flow:			kTok_flow
				{
					WriteStatus("flow");
					currentFlow = new Flow();
					theVisualObjects.push_back(currentFlow);
					currentObject = currentFlow;
					calcStages.push(currentFlow);
				}
				field_expression 
				{
					currentFlow->SetData($3);
				}
				flow_options ';'
				{
					calcStages.pop();
					currentFlow = NULL;
					currentObject = NULL;
					WriteDone();
				}
flow_options:
			|	flow_options flow_option
			;

flow_option:	object_option
			|	kTok_at point_list
				{
					currentFlow->SetPoints($2);
				}
			|	kTok_length value_expression
				{
					currentFlow->SetLength($2);
				}
			|	kTok_stepsize expression
				{
					currentFlow->SetStepsize($2);
				}
			;
				
point_list:		explicit_point_list
			|	kTok_grid value_expression ',' value_expression ',' value_expression
				{
					$$ = autorelease(new GridPointList($2,$4,$6));
				}
			//|	kTok_slice xyz '=' value_expression kTok_spacing value_expression
			;

explicit_point_list:
				value_expression
				{
					$$ = autorelease(new ExplicitPointList());
					((ExplicitPointList*)$$)->AddPoint($1);
				}
			|	explicit_point_list ',' value_expression
				{
					((ExplicitPointList*)$1)->AddPoint($3);
					$$ = $1;
				}
			;

object_option:	kTok_color field_expression
				{
					currentObject->SetColorFunction($2);
				}
			|	kTok_cutout '<' range_specifier ',' range_specifier ',' range_specifier '>'
				{
					currentObject->SetCutout($3,$5,$7);
				}
			|	kTok_transparency 
				{
					calcStages.push(VisualObject::BSPMaterialUpdater);
				}
				value_expression
				{
					calcStages.pop();
					currentObject->SetTransparency($3);
				}
			|	kTok_shininess
				{
					calcStages.push(VisualObject::BSPMaterialUpdater);
				}
				value_expression
				{
					calcStages.pop();
					currentObject->SetShininess($3);
				}
			;

animation:		kTok_animate 
				{
					anim.sections.clear();
					animSection.vars.clear();
				}
				animation_list ';'
				{
					//anim.GenerateAnimation();
					theAnimations.push_back(new Animator(anim));
					anim.sections.clear();
					
					std::cout << "NOTE:  The file contained information about an animation.\n";
					std::cout << "       Hit 'a' to generate the animation.\n";
				}
			;
			
animation_list:	animation_section
			|	animation_list and_or_comma animation_section
			;
			
animation_section:
				animation_var_list kTok_in sequence_dur
				{
					animSection.frames = $3;
					anim.sections.push_back(animSection);
					animSection.vars.clear();
				}
			;
			
sequence_dur:	positive_integer_constant kTok_frames
			|	positive_integer_constant kTok_frame
			|	positive_integer_constant
			;
			
animation_var_list:
				animation_var_spec
			|	animation_var_list and_or_comma animation_var_spec
			;
			
animation_var_spec:
				kTokRealVariableIdentifier kTok_from real_constant kTok_to real_constant
				{
					AnimatorVariableSpec animVar;
					animVar.var = GetRealVar($1);
					animVar.range.first = $3;
					animVar.range.second = $5;
					animVar.inclusive = false;
					animSection.vars.push_back(animVar);
				}
			|	kTokRealVariableIdentifier kTok_from real_constant kTok_to real_constant
					kTok_inclusive
				{
					AnimatorVariableSpec animVar;
					animVar.var = GetRealVar($1);
					animVar.range.first = $3;
					animVar.range.second = $5;
					animVar.inclusive = true;
					animSection.vars.push_back(animVar);
				}
			;

and_or_comma:	kTok_and
			|	','
			;
			
assignment:		kTokRealVariableIdentifier kTokAssign real_constant ';'
				{
					GetRealVar($1)->ChangeTo($3);
				}
			;
			
/*window_directive:
				kTok_window positive_integer_constant ',' positive_integer_constant ';'
				{
					ResizeDisplayTo($2,$4);
				}
			;*/

box_directive:	kTok_nobox ';'
				{
					theSceneOptions.boxType = kBoxNone;
				}
			|	kTok_colorbox ';'
				{
					theSceneOptions.boxType = kBoxColored;
				}
			|	kTok_colorbox constant_color ';'
				{
					theSceneOptions.boxType = kBoxOneCustomColor;
					theSceneOptions.boxColorX = $2;
				}
			|	kTok_colorbox constant_color ',' constant_color ',' constant_color ';'
				{
					theSceneOptions.boxType = kBoxThreeCustomColors;
					theSceneOptions.boxColorX = $2;
					theSceneOptions.boxColorY = $4;
					theSceneOptions.boxColorZ = $6;
				}
			|	kTok_grid positive_integer_constant ';'
				{
					theSceneOptions.boxDivisions = $2;
				}
			|	kTok_world_size range_or_diameter ';'
				{
					theSceneOptions.xr =
						theSceneOptions.yr =
						theSceneOptions.zr = $2;
				}
			|	kTok_world_size '<' range_or_diameter ',' range_or_diameter ',' range_or_diameter '>' ';'
				{
					theSceneOptions.xr = $3;
					theSceneOptions.yr = $5;
					theSceneOptions.zr = $7;
				}
			;

resolution_directive:
				kTok_resolution positive_integer_constant ';'
				{
					vecN3 res = {$2,$2,$2};
					ExpressionEvaluator::SetGlobalDefaultResolution(res);
				}
			;

fog_directive:
				kTok_fog range_specifier ';'
				{
					theSceneOptions.fogRange = $2;
				}
			;

background_directive:
				kTok_background constant_color ';'
				{
					theSceneOptions.backgroundColor = $2;
				}
			;
			
constant_color:
				'<' real_constant ',' real_constant ',' real_constant '>'
				{
					$$.x = $2;
					$$.y = $4;
					$$.z = $6;
				}
			;

%%


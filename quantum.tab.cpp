
/*  A Bison parser, made from quantum.y
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define	kTok_variable	257
#define	kTok_field	258
#define	kTok_purge	259
#define	kTok_isosurface	260
#define	kTok_slice	261
#define	kTok_vectors	262
#define	kTok_flow	263
#define	kTok_window	264
#define	kTok_nobox	265
#define	kTok_colorbox	266
#define	kTok_grid	267
#define	kTok_world_size	268
#define	kTok_fog	269
#define	kTok_background	270
#define	kTok_min	271
#define	kTok_max	272
#define	kTok_cx	273
#define	kTok_cy	274
#define	kTok_cz	275
#define	kTok_resolution	276
#define	kTok_at	277
#define	kTok_cyclic	278
#define	kTok_color	279
#define	kTok_cutout	280
#define	kTok_transparency	281
#define	kTok_framed	282
#define	kTok_shininess	283
#define	kTok_secondary_color	284
#define	kTok_sphere_color	285
#define	kTok_sphere_radius	286
#define	kTok_length	287
#define	kTok_stepsize	288
#define	kTok_x	289
#define	kTok_y	290
#define	kTok_z	291
#define	kTok_i	292
#define	kTok_read	293
#define	kTok_abs	294
#define	kTok_zoom	295
#define	kTok_scaled_to	296
#define	kTok_cut_to	297
#define	kTok_filled_to	298
#define	kTok_with	299
#define	kTokAssign	300
#define	kTokIdentifier	301
#define	kTokReal	302
#define	kTokPosInteger	303
#define	kTokString	304
#define	kTokFieldIdentifier	305
#define	kTokRealVariableIdentifier	306
#define	kTokFieldFunctionIdentifier	307
#define	kTokStandardFunctionIdentifier	308
#define	kTok_animate	309
#define	kTok_from	310
#define	kTok_to	311
#define	kTok_and	312
#define	kTok_in	313
#define	kTok_frames	314
#define	kTok_frame	315
#define	kTok_inclusive	316

#line 1 "quantum.y"

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

#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		274
#define	YYFLAG		-32768
#define	YYNTBASE	77

#define YYTRANSLATE(x) ((unsigned)(x) <= 316 ? yytranslate[x] : 139)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    74,
    75,    71,    70,    65,    67,    73,    72,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,    63,    68,
    76,    69,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    64,     2,    66,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     3,     4,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
    37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
    47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
    57,    58,    59,    60,    61,    62
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     1,     4,     6,     8,    10,    12,    14,    16,    18,
    20,    22,    24,    26,    28,    35,    41,    42,    44,    46,
    48,    50,    53,    55,    57,    59,    61,    63,    65,    69,
    79,    83,    93,    95,    99,   103,   106,   108,   112,   116,
   120,   124,   126,   128,   130,   131,   136,   141,   146,   151,
   156,   160,   164,   166,   167,   173,   174,   176,   178,   182,
   184,   185,   192,   193,   202,   203,   204,   211,   212,   215,
   217,   220,   223,   224,   229,   230,   233,   235,   239,   241,
   243,   245,   247,   248,   249,   256,   257,   260,   262,   265,
   268,   271,   274,   275,   276,   283,   284,   287,   289,   292,
   295,   298,   300,   307,   309,   313,   316,   325,   326,   330,
   331,   335,   336,   341,   343,   347,   351,   354,   357,   359,
   361,   365,   371,   378,   380,   382,   387,   390,   393,   397,
   405,   409,   413,   423,   427,   431,   435
};

static const short yyrhs[] = {    -1,
    77,    78,     0,    79,     0,    97,     0,   100,     0,   105,
     0,   110,     0,   115,     0,   125,     0,   133,     0,   134,
     0,   135,     0,   136,     0,   137,     0,     3,    47,    81,
    46,    83,    63,     0,    64,    83,    65,    83,    66,     0,
     0,    80,     0,    80,     0,    83,     0,    84,     0,    67,
    84,     0,    48,     0,    85,     0,    49,     0,    88,     0,
    88,     0,    89,     0,    87,    42,    82,     0,    87,    42,
    68,    82,    65,    82,    65,    82,    69,     0,    87,    43,
    82,     0,    87,    43,    68,    82,    65,    82,    65,    82,
    69,     0,    90,     0,    88,    70,    90,     0,    88,    67,
    90,     0,    67,    90,     0,    91,     0,    90,    71,    91,
     0,    90,    72,    91,     0,    90,    73,    91,     0,    74,
    88,    75,     0,    52,     0,    84,     0,    38,     0,     0,
    68,    92,    95,    69,     0,    54,    74,    88,    75,     0,
     4,    74,    88,    75,     0,    39,    74,    50,    75,     0,
    40,    74,    87,    75,     0,    51,    73,    17,     0,    51,
    73,    18,     0,    51,     0,     0,    53,    93,    74,    94,
    75,     0,     0,    95,     0,    96,     0,    95,    65,    96,
     0,    88,     0,     0,     4,    47,    46,    98,    87,    63,
     0,     0,     4,    47,    22,    85,    46,    99,    87,    63,
     0,     0,     0,     6,   101,    87,   102,   103,    63,     0,
     0,   103,   104,     0,   122,     0,    23,    86,     0,    24,
    83,     0,     0,     7,   106,   107,    63,     0,     0,   107,
   108,     0,   122,     0,   109,    76,    86,     0,    28,     0,
    35,     0,    36,     0,    37,     0,     0,     0,     8,   111,
    87,   112,   113,    63,     0,     0,   113,   114,     0,   122,
     0,    30,    87,     0,    31,    87,     0,    32,    87,     0,
    23,   120,     0,     0,     0,     9,   116,    87,   117,   118,
    63,     0,     0,   118,   119,     0,   122,     0,    23,   120,
     0,    33,    86,     0,    34,    88,     0,   121,     0,    13,
    86,    65,    86,    65,    86,     0,    86,     0,   121,    65,
    86,     0,    25,    87,     0,    26,    68,    80,    65,    80,
    65,    80,    69,     0,     0,    27,   123,    86,     0,     0,
    29,   124,    86,     0,     0,    55,   126,   127,    63,     0,
   128,     0,   127,   132,   128,     0,   130,    59,   129,     0,
    85,    60,     0,    85,    61,     0,    85,     0,   131,     0,
   130,   132,   131,     0,    52,    56,    83,    57,    83,     0,
    52,    56,    83,    57,    83,    62,     0,    58,     0,    65,
     0,    52,    46,    83,    63,     0,    11,    63,     0,    12,
    63,     0,    12,   138,    63,     0,    12,   138,    65,   138,
    65,   138,    63,     0,    13,    85,    63,     0,    14,    82,
    63,     0,    14,    68,    82,    65,    82,    65,    82,    69,
    63,     0,    22,    85,    63,     0,    15,    80,    63,     0,
    16,   138,    63,     0,    68,    83,    65,    83,    65,    83,
    69,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   211,   227,   230,   231,   232,   233,   234,   235,   236,   237,
   239,   240,   241,   242,   245,   257,   265,   269,   272,   274,
   280,   281,   287,   288,   304,   321,   330,   344,   345,   349,
   354,   358,   365,   366,   370,   374,   380,   381,   385,   389,
   395,   398,   404,   415,   419,   423,   438,   450,   459,   463,
   472,   480,   488,   495,   499,   506,   507,   510,   511,   514,
   520,   526,   536,   542,   554,   565,   576,   586,   587,   590,
   592,   596,   602,   610,   620,   621,   624,   625,   633,   639,
   641,   643,   647,   656,   659,   667,   668,   671,   672,   676,
   680,   684,   690,   699,   702,   709,   710,   713,   714,   718,
   722,   728,   729,   736,   742,   749,   753,   757,   762,   766,
   771,   777,   782,   793,   794,   797,   806,   807,   808,   811,
   813,   816,   826,   838,   839,   842,   855,   859,   863,   868,
   875,   879,   885,   893,   901,   908,   915
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","kTok_variable",
"kTok_field","kTok_purge","kTok_isosurface","kTok_slice","kTok_vectors","kTok_flow",
"kTok_window","kTok_nobox","kTok_colorbox","kTok_grid","kTok_world_size","kTok_fog",
"kTok_background","kTok_min","kTok_max","kTok_cx","kTok_cy","kTok_cz","kTok_resolution",
"kTok_at","kTok_cyclic","kTok_color","kTok_cutout","kTok_transparency","kTok_framed",
"kTok_shininess","kTok_secondary_color","kTok_sphere_color","kTok_sphere_radius",
"kTok_length","kTok_stepsize","kTok_x","kTok_y","kTok_z","kTok_i","kTok_read",
"kTok_abs","kTok_zoom","kTok_scaled_to","kTok_cut_to","kTok_filled_to","kTok_with",
"kTokAssign","kTokIdentifier","kTokReal","kTokPosInteger","kTokString","kTokFieldIdentifier",
"kTokRealVariableIdentifier","kTokFieldFunctionIdentifier","kTokStandardFunctionIdentifier",
"kTok_animate","kTok_from","kTok_to","kTok_and","kTok_in","kTok_frames","kTok_frame",
"kTok_inclusive","';'","'['","','","']'","'-'","'<'","'>'","'+'","'*'","'/'",
"'.'","'('","')'","'='","input","statement","variable_decl","range_specifier",
"optional_range_specifier","range_or_diameter","real_constant","basic_real_constant",
"positive_integer_constant","value_expression","field_expression","expression",
"arithmetic","term","primary","@1","@2","ff_field_args","ff_field_args1","ff_field_arg",
"field_decl","@3","@4","isosurface","@5","@6","isosurface_options","isosurface_option",
"slice","@7","slice_options","slice_option","xyz","vectors","@8","@9","vectors_options",
"vectors_option","flow","@10","@11","flow_options","flow_option","point_list",
"explicit_point_list","object_option","@12","@13","animation","@14","animation_list",
"animation_section","sequence_dur","animation_var_list","animation_var_spec",
"and_or_comma","assignment","box_directive","resolution_directive","fog_directive",
"background_directive","constant_color", NULL
};
#endif

static const short yyr1[] = {     0,
    77,    77,    78,    78,    78,    78,    78,    78,    78,    78,
    78,    78,    78,    78,    79,    80,    81,    81,    82,    82,
    83,    83,    84,    84,    85,    86,    87,    88,    88,    88,
    88,    88,    89,    89,    89,    89,    90,    90,    90,    90,
    91,    91,    91,    91,    92,    91,    91,    91,    91,    91,
    91,    91,    91,    93,    91,    94,    94,    95,    95,    96,
    98,    97,    99,    97,   101,   102,   100,   103,   103,   104,
   104,   104,   106,   105,   107,   107,   108,   108,   108,   109,
   109,   109,   111,   112,   110,   113,   113,   114,   114,   114,
   114,   114,   116,   117,   115,   118,   118,   119,   119,   119,
   119,   120,   120,   121,   121,   122,   122,   123,   122,   124,
   122,   126,   125,   127,   127,   128,   129,   129,   129,   130,
   130,   131,   131,   132,   132,   133,   134,   134,   134,   134,
   134,   134,   134,   135,   136,   137,   138
};

static const short yyr2[] = {     0,
     0,     2,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     6,     5,     0,     1,     1,     1,
     1,     2,     1,     1,     1,     1,     1,     1,     3,     9,
     3,     9,     1,     3,     3,     2,     1,     3,     3,     3,
     3,     1,     1,     1,     0,     4,     4,     4,     4,     4,
     3,     3,     1,     0,     5,     0,     1,     1,     3,     1,
     0,     6,     0,     8,     0,     0,     6,     0,     2,     1,
     2,     2,     0,     4,     0,     2,     1,     3,     1,     1,
     1,     1,     0,     0,     6,     0,     2,     1,     2,     2,
     2,     2,     0,     0,     6,     0,     2,     1,     2,     2,
     2,     1,     6,     1,     3,     2,     8,     0,     3,     0,
     3,     0,     4,     1,     3,     3,     2,     2,     1,     1,
     3,     5,     6,     1,     1,     4,     2,     2,     3,     7,
     3,     3,     9,     3,     3,     3,     7
};

static const short yydefact[] = {     1,
     0,     0,     0,    65,    73,    83,    93,     0,     0,     0,
     0,     0,     0,     0,     0,   112,     2,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    17,
     0,     0,    75,     0,     0,   127,   128,     0,     0,    25,
     0,    23,     0,     0,     0,    19,     0,    20,    21,    24,
     0,     0,     0,     0,     0,    18,     0,     0,    61,     0,
    44,     0,     0,    53,    42,    54,     0,     0,    45,     0,
    43,    66,    27,    28,    33,    37,     0,    84,    94,     0,
   129,     0,   131,     0,    22,     0,   132,   135,   136,   134,
     0,     0,     0,   114,     0,   120,     0,     0,     0,     0,
     0,     0,     0,     0,     0,    36,     0,     0,    27,     0,
     0,    68,     0,     0,     0,     0,     0,     0,     0,   108,
    79,   110,    80,    81,    82,    74,    76,     0,    77,    86,
    96,     0,     0,     0,     0,   126,     0,   124,   113,   125,
     0,     0,     0,     0,    63,     0,    27,     0,     0,    51,
    52,    56,    27,    60,     0,    58,    41,     0,    29,     0,
    31,     0,    35,    34,    38,    39,    40,   106,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,   115,
   119,   116,   121,    15,     0,    62,    48,    49,    50,     0,
    57,    47,     0,    46,     0,     0,     0,     0,    67,    69,
    70,     0,   109,    26,   111,    78,     0,     0,     0,     0,
    85,    87,    88,     0,     0,     0,    95,    97,    98,     0,
     0,    16,     0,     0,   117,   118,     0,    55,    59,     0,
     0,    71,    72,     0,     0,   104,    92,   102,    89,    90,
    91,    99,   100,   101,     0,   130,     0,   122,    64,     0,
     0,     0,     0,     0,   137,     0,   123,     0,     0,     0,
     0,   105,   133,     0,     0,     0,     0,    30,    32,   107,
     0,   103,     0,     0
};

static const short yydefgoto[] = {     1,
    17,    18,    46,    57,    47,    48,    71,    50,   236,   108,
   204,    74,    75,    76,   107,   104,   190,   155,   156,    19,
    99,   185,    20,    32,   112,   162,   200,    21,    33,    77,
   127,   128,    22,    34,   130,   173,   212,    23,    35,   131,
   174,   218,   237,   238,   129,   170,   171,    24,    55,    93,
    94,   182,    95,    96,   141,    25,    26,    27,    28,    29,
    39
};

static const short yypact[] = {-32768,
   322,    -1,     4,-32768,-32768,-32768,-32768,   -53,   -27,    -7,
   129,    -2,     8,    -7,    11,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    -2,
   -16,   253,-32768,   253,   253,-32768,-32768,    23,    45,-32768,
     3,-32768,    23,    71,    15,-32768,    17,-32768,-32768,-32768,
    34,    42,    48,    23,    65,-32768,    82,    -7,-32768,    20,
-32768,    70,    72,    83,-32768,-32768,    84,   131,-32768,   253,
-32768,    98,    14,-32768,    94,-32768,   204,    98,    98,    95,
-32768,     8,-32768,   121,-32768,   127,-32768,-32768,-32768,-32768,
    73,   144,    69,-32768,   -37,-32768,    23,   155,   253,   253,
   152,   253,   145,   130,   253,    94,   253,    98,   -44,   215,
   217,-32768,   131,   131,   131,   131,   131,   253,   135,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,   132,-32768,-32768,
-32768,    23,   141,    23,    15,-32768,    23,-32768,-32768,-32768,
    65,    -7,    65,   147,-32768,   -28,    39,   136,   -31,-32768,
-32768,   253,    63,    31,   -40,-32768,-32768,    15,-32768,    15,
-32768,   128,    94,    94,-32768,-32768,-32768,    98,    -2,   253,
   253,   253,    29,    62,   142,     8,   148,   150,   161,-32768,
   115,-32768,-32768,-32768,   253,-32768,-32768,-32768,-32768,   159,
   160,-32768,   253,-32768,   162,   172,   253,    23,-32768,-32768,
-32768,   173,-32768,    31,-32768,-32768,   222,   253,   253,   253,
-32768,-32768,-32768,   222,   253,   253,-32768,-32768,-32768,    23,
   179,-32768,    15,    23,-32768,-32768,   -26,-32768,-32768,    15,
    15,-32768,-32768,    -2,   253,-32768,-32768,   178,    98,    98,
    98,-32768,-32768,    31,   175,-32768,   176,   184,-32768,   189,
   190,   191,   193,   253,-32768,   186,-32768,    15,    15,    -2,
   253,-32768,-32768,   199,   200,   203,   212,-32768,-32768,-32768,
   253,-32768,   278,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,   -10,-32768,   -42,   -30,   -11,    -5,  -132,    13,
    43,-32768,   -64,    57,-32768,-32768,-32768,   134,    87,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,    74,-32768,  -155,-32768,-32768,-32768,-32768,-32768,
   146,-32768,-32768,   151,   202,-32768,-32768,-32768,-32768,-32768,
   -12
};


#define	YYLAST		377


static const short yytable[] = {    49,
    52,    51,    86,   106,    41,    58,   201,    80,    53,    36,
   110,   111,    84,   110,   111,   110,   111,   213,   219,    56,
   138,   142,   113,    91,   193,   114,    49,   140,   194,    59,
   157,    49,    85,    49,   186,    37,   249,   203,   205,   206,
    38,    40,    49,   189,    72,    30,    78,    79,   163,   164,
    31,   207,    98,   118,   119,   120,    54,   122,   208,   209,
   210,    43,    42,    40,   232,    83,   144,   159,   161,   133,
    42,    40,   -27,   -27,    73,    38,    73,    73,    43,    87,
   113,    44,   243,   114,   214,    49,   118,   119,   120,    44,
   122,   211,   178,   100,   215,   216,    88,   113,    49,    49,
   114,   175,   253,   177,    89,   113,   179,    81,   114,    82,
    90,   146,   109,   187,   149,   195,    92,   196,    42,    40,
    49,   262,    49,    49,   217,    49,   138,    97,   267,   113,
   168,   139,   114,   140,    60,   136,   181,   192,   272,   110,
   111,    73,   147,   101,    73,   102,    49,   153,    49,   154,
   197,   198,   118,   119,   120,   103,   122,   105,   202,   132,
    73,   150,   151,   221,   115,   116,   117,   233,    61,    62,
    63,   165,   166,   167,   225,   226,    42,    40,    42,    40,
   247,    64,    65,    66,    67,   134,    49,   250,   251,   245,
   199,   135,    43,   248,   154,    44,    45,   227,    69,   137,
   145,   148,   169,   152,    70,   176,   220,   172,    49,   184,
   188,    49,    49,   222,   223,   264,   265,   224,    49,    49,
   239,   240,   241,   252,   193,    60,   230,    73,   118,   119,
   120,   121,   122,   228,   235,   154,   231,   234,   123,   124,
   125,   246,   254,   255,   256,   257,    49,    49,   263,   266,
    73,    73,    73,   258,   259,   260,    60,   261,   244,    61,
    62,    63,    42,    40,    42,    40,   126,   268,   269,    42,
    40,   270,    64,    65,    66,    67,   271,   274,    43,   229,
    43,    44,   158,    44,   160,   191,   180,   242,    68,    69,
    61,    62,    63,   183,     0,    70,   143,     0,     0,     0,
    42,    40,     0,    64,    65,    66,    67,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,    68,
    69,   273,     0,     0,     2,     3,    70,     4,     5,     6,
     7,     0,     8,     9,    10,    11,    12,    13,     0,     0,
     0,     0,     0,    14,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,    15,     0,     0,    16
};

static const short yycheck[] = {    11,
    13,    12,    45,    68,    10,    22,   162,    38,    14,    63,
    42,    43,    43,    42,    43,    42,    43,   173,   174,    30,
    58,    59,    67,    54,    65,    70,    38,    65,    69,    46,
    75,    43,    44,    45,    63,    63,    63,   170,   171,   172,
    68,    49,    54,    75,    32,    47,    34,    35,   113,   114,
    47,    23,    58,    25,    26,    27,    46,    29,    30,    31,
    32,    64,    48,    49,   197,    63,    97,   110,   111,    82,
    48,    49,    42,    43,    32,    68,    34,    35,    64,    63,
    67,    67,   215,    70,    23,    97,    25,    26,    27,    67,
    29,    63,   135,    74,    33,    34,    63,    67,   110,   111,
    70,   132,   235,   134,    63,    67,   137,    63,    70,    65,
    63,    99,    70,    75,   102,   158,    52,   160,    48,    49,
   132,   254,   134,   135,    63,   137,    58,    46,   261,    67,
   118,    63,    70,    65,     4,    63,   142,    75,   271,    42,
    43,    99,   100,    74,   102,    74,   158,   105,   160,   107,
    23,    24,    25,    26,    27,    73,    29,    74,   169,    65,
   118,    17,    18,   176,    71,    72,    73,   198,    38,    39,
    40,   115,   116,   117,    60,    61,    48,    49,    48,    49,
   223,    51,    52,    53,    54,    65,   198,   230,   231,   220,
    63,    65,    64,   224,   152,    67,    68,   185,    68,    56,
    46,    50,    68,    74,    74,    65,    65,    76,   220,    63,
    75,   223,   224,    66,    65,   258,   259,    57,   230,   231,
   208,   209,   210,   234,    65,     4,    65,   185,    25,    26,
    27,    28,    29,    75,    13,   193,    65,    65,    35,    36,
    37,    63,    65,    69,    69,    62,   258,   259,    63,   260,
   208,   209,   210,    65,    65,    65,     4,    65,   216,    38,
    39,    40,    48,    49,    48,    49,    63,    69,    69,    48,
    49,    69,    51,    52,    53,    54,    65,     0,    64,   193,
    64,    67,    68,    67,    68,   152,   141,   214,    67,    68,
    38,    39,    40,   143,    -1,    74,    95,    -1,    -1,    -1,
    48,    49,    -1,    51,    52,    53,    54,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    67,
    68,     0,    -1,    -1,     3,     4,    74,     6,     7,     8,
     9,    -1,    11,    12,    13,    14,    15,    16,    -1,    -1,
    -1,    -1,    -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    52,    -1,    -1,    55
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/share/bison.simple"
/* This file comes from bison-1.28.  */

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

#ifndef YYSTACK_USE_ALLOCA
#ifdef alloca
#define YYSTACK_USE_ALLOCA
#else /* alloca not defined */
#ifdef __GNUC__
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi) || (defined (__sun) && defined (__i386))
#define YYSTACK_USE_ALLOCA
#include <alloca.h>
#else /* not sparc */
/* We think this test detects Watcom and Microsoft C.  */
/* This used to test MSDOS, but that is a bad idea
   since that symbol is in the user namespace.  */
#if (defined (_MSDOS) || defined (_MSDOS_)) && !defined (__TURBOC__)
#if 0 /* No need for malloc.h, which pollutes the namespace;
	 instead, just don't use alloca.  */
#include <malloc.h>
#endif
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
/* I don't know what this was needed for, but it pollutes the namespace.
   So I turned it off.   rms, 2 May 1997.  */
/* #include <malloc.h>  */
 #pragma alloca
#define YYSTACK_USE_ALLOCA
#else /* not MSDOS, or __TURBOC__, or _AIX */
#if 0
#ifdef __hpux /* haible@ilog.fr says this works for HPUX 9.05 and up,
		 and on HPUX 10.  Eventually we can turn this on.  */
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#endif /* __hpux */
#endif
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc */
#endif /* not GNU C */
#endif /* alloca not defined */
#endif /* YYSTACK_USE_ALLOCA not defined */

#ifdef YYSTACK_USE_ALLOCA
#define YYSTACK_ALLOC alloca
#else
#define YYSTACK_ALLOC malloc
#endif

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Define __yy_memcpy.  Note that the size argument
   should be passed with type unsigned int, because that is what the non-GCC
   definitions require.  With GCC, __builtin_memcpy takes an arg
   of type size_t, but it can handle unsigned int.  */

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     unsigned int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, unsigned int count)
{
  register char *t = to;
  register char *f = from;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 217 "/usr/share/bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
#ifdef YYPARSE_PARAM
int yyparse (void *);
#else
int yyparse (void);
#endif
#endif

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;
  int yyfree_stacks = 0;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  if (yyfree_stacks)
	    {
	      free (yyss);
	      free (yyvs);
#ifdef YYLSP_NEEDED
	      free (yyls);
#endif
	    }
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
#ifndef YYSTACK_USE_ALLOCA
      yyfree_stacks = 1;
#endif
      yyss = (short *) YYSTACK_ALLOC (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1,
		   size * (unsigned int) sizeof (*yyssp));
      yyvs = (YYSTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1,
		   size * (unsigned int) sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1,
		   size * (unsigned int) sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 1:
#line 212 "quantum.y"
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
				;
    break;}
case 15:
#line 247 "quantum.y"
{
					RealVariable *var = new RealVariable;
					var->name = yyvsp[-4].str;
					var->constant = false;
					var->theRange = yyvsp[-3].range;
					var->value = yyvsp[-1].realNum;
					DefineRealVariable(var);
				;
    break;}
case 16:
#line 259 "quantum.y"
{
					yyval.range = Range(yyvsp[-3].realNum,yyvsp[-1].realNum);
				;
    break;}
case 17:
#line 266 "quantum.y"
{
					yyval.range = fullRange;
				;
    break;}
case 20:
#line 275 "quantum.y"
{
					yyval.range = Range(- yyvsp[0].realNum / 2, yyvsp[0].realNum / 2);
				;
    break;}
case 22:
#line 282 "quantum.y"
{
					yyval.realNum = -yyvsp[0].realNum;
				;
    break;}
case 24:
#line 289 "quantum.y"
{
					yyval.realNum = (number) yyvsp[0].integer;
				;
    break;}
case 26:
#line 323 "quantum.y"
{
					ValueEvaluator *expr = dynamic_cast<ValueEvaluator*> (yyvsp[0].expr);
					assert(expr); //###
					yyval.value = expr;
				;
    break;}
case 27:
#line 332 "quantum.y"
{
					if(ExpressionEvaluator *expr = dynamic_cast<ExpressionEvaluator*> (yyvsp[0].expr))
						yyval.field = expr;
					else
					{
						ValueEvaluator *val = dynamic_cast<ValueEvaluator*> (yyvsp[0].expr);
						assert(val);
						yyval.field = autorelease(new ConstantFieldEvaluator(val));
					}
				;
    break;}
case 29:
#line 346 "quantum.y"
{
					yyval.expr = autorelease(new ScaleToEvaluator(yyvsp[-2].field,yyvsp[0].range,yyvsp[0].range,yyvsp[0].range));
				;
    break;}
case 30:
#line 351 "quantum.y"
{
					yyval.expr = autorelease(new ScaleToEvaluator(yyvsp[-8].field,yyvsp[-5].range,yyvsp[-3].range,yyvsp[-1].range));
				;
    break;}
case 31:
#line 355 "quantum.y"
{
					yyval.expr = autorelease(new CutToEvaluator(yyvsp[-2].field,yyvsp[0].range,yyvsp[0].range,yyvsp[0].range));
				;
    break;}
case 32:
#line 360 "quantum.y"
{
					yyval.expr = autorelease(new CutToEvaluator(yyvsp[-8].field,yyvsp[-5].range,yyvsp[-3].range,yyvsp[-1].range));
				;
    break;}
case 34:
#line 367 "quantum.y"
{
					yyval.expr = autorelease(CreateBinaryOp(StdBinValEvalFactory<AddValueEvaluator>(),yyvsp[-2].expr,yyvsp[0].expr,false));
				;
    break;}
case 35:
#line 371 "quantum.y"
{
					yyval.expr = autorelease(CreateBinaryOp(StdBinValEvalFactory<SubtractValueEvaluator>(),yyvsp[-2].expr,yyvsp[0].expr,false));
				;
    break;}
case 36:
#line 375 "quantum.y"
{
					yyval.expr = autorelease(CreateNegateOp(yyvsp[0].expr));
				;
    break;}
case 38:
#line 382 "quantum.y"
{
					yyval.expr = autorelease(CreateBinaryOp(StdBinValEvalFactory<MultiplyValueEvaluator>(),yyvsp[-2].expr,yyvsp[0].expr,true));
				;
    break;}
case 39:
#line 386 "quantum.y"
{
					yyval.expr = autorelease(CreateBinaryOp(StdBinValEvalFactory<DivideValueEvaluator>(),yyvsp[-2].expr,yyvsp[0].expr,true));
				;
    break;}
case 40:
#line 390 "quantum.y"
{
					yyval.expr = autorelease(CreateBinaryOp(StdBinValEvalFactory<DotValueEvaluator>(),yyvsp[-2].expr,yyvsp[0].expr,true));
				;
    break;}
case 41:
#line 396 "quantum.y"
{ yyval.expr = yyvsp[-1].expr; ;
    break;}
case 42:
#line 399 "quantum.y"
{
					RealVariable *var = GetRealVar(yyvsp[0].str);
					DependOnVar(var);
					yyval.expr = var;
				;
    break;}
case 43:
#line 405 "quantum.y"
{
					RealVariable *var = new RealVariable;
					var->name = "";
					var->constant = true;
					var->value = yyvsp[0].realNum;
					var->theRange = Range(yyvsp[0].realNum,yyvsp[0].realNum);
					//unnamedVariables.push_back(var);
					yyval.expr = autorelease(var);
				;
    break;}
case 44:
#line 416 "quantum.y"
{
					yyval.expr = autorelease(new ImaginaryUnitConstant());
				;
    break;}
case 45:
#line 420 "quantum.y"
{
					funcParams.push(vector<CommonEvaluator*>());
				;
    break;}
case 46:
#line 424 "quantum.y"
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
					yyval.expr = autorelease(e);
				;
    break;}
case 47:
#line 439 "quantum.y"
{
					if(ValueEvaluator *val = dynamic_cast<ValueEvaluator*> (yyvsp[-1].expr))
						yyval.expr = autorelease(CreateStandardFunctionEvaluator(yyvsp[-3].str,val));
					else
					{
						ExpressionEvaluator *expr = dynamic_cast<ExpressionEvaluator*> (yyvsp[-1].expr);
						assert(expr);

						assert(false);
					}
				;
    break;}
case 48:
#line 451 "quantum.y"
{
					if(ValueEvaluator *val = dynamic_cast<ValueEvaluator*> (yyvsp[-1].expr))
					{
						yyval.expr = autorelease(new ConstantFieldEvaluator(val));
					}
					else
						throw TypeMismatch(); //### $$ = $3; ?
				;
    break;}
case 49:
#line 460 "quantum.y"
{
					yyval.expr = autorelease(new ReadEvaluator(yyvsp[-1].str.c_str()));
				;
    break;}
case 50:
#line 464 "quantum.y"
{
					yyval.expr = autorelease(new AbsEvaluator(yyvsp[-1].field));
				;
    break;}
case 51:
#line 473 "quantum.y"
{
					DefinedField *defined = GetFieldVar(yyvsp[-2].str);
					RealVariable *val = defined->GetMinEvaluator();
					assert(!calcStages.empty());
					calcStages.top()->DependOn(val);
					yyval.expr = val;
				;
    break;}
case 52:
#line 481 "quantum.y"
{
					DefinedField *defined = GetFieldVar(yyvsp[-2].str);
					RealVariable *val = defined->GetMaxEvaluator();
					assert(!calcStages.empty());
					calcStages.top()->DependOn(val);
					yyval.expr = val;
				;
    break;}
case 53:
#line 489 "quantum.y"
{
					DefinedField *defined = GetFieldVar(yyvsp[0].str);
					assert(!calcStages.empty());
					calcStages.top()->DependOn(defined);
					yyval.expr = autorelease(new ExpressionEvaluator(defined));
				;
    break;}
case 54:
#line 496 "quantum.y"
{
					funcParams.push(vector<CommonEvaluator*>());
				;
    break;}
case 55:
#line 500 "quantum.y"
{
					yyval.expr = autorelease(FindFieldFunction(yyvsp[-4].str,funcParams.top()));
					funcParams.pop();
				;
    break;}
case 60:
#line 515 "quantum.y"
{
					funcParams.top().push_back(yyvsp[0].expr);
				;
    break;}
case 61:
#line 522 "quantum.y"
{
					currentDefinedField = new DefinedField(yyvsp[-1].str,0);
					calcStages.push(currentDefinedField);
				;
    break;}
case 62:
#line 527 "quantum.y"
{
					WriteStatus("field " + yyvsp[-4].str);
					currentDefinedField->SetExpr(yyvsp[-1].field);
					definedFields[yyvsp[-4].str] = currentDefinedField;
					currentDefinedField = NULL;
					calcStages.pop();
					RegisterIdentifierAs(yyvsp[-4].str,kTokFieldIdentifier);
					WriteDone();
				;
    break;}
case 63:
#line 538 "quantum.y"
{
					currentDefinedField = new DefinedField(yyvsp[-3].str,yyvsp[-1].integer);
					calcStages.push(currentDefinedField);
				;
    break;}
case 64:
#line 543 "quantum.y"
{
					WriteStatus("field " + yyvsp[-6].str);
					currentDefinedField->SetExpr(yyvsp[-1].field);
					definedFields[yyvsp[-6].str] = currentDefinedField;
					currentDefinedField = NULL;
					calcStages.pop();
					RegisterIdentifierAs(yyvsp[-6].str,kTokFieldIdentifier);
					WriteDone();
				;
    break;}
case 65:
#line 555 "quantum.y"
{
					WriteStatus("isosurface");
					currentSurface = new IsoSurface();
					theVisualObjects.push_back(currentSurface);
					currentObject = currentSurface;
					calcStages.push(currentSurface);
					
					calcStages.push(currentSurface->GetFieldEvaluationStage());
				;
    break;}
case 66:
#line 565 "quantum.y"
{
					calcStages.pop();
					
					currentSurface->SetData(yyvsp[0].field);
					
					Range xr,yr,zr;
					yyvsp[0].field->GetRanges(xr,yr,zr);
					cout << "([" << xr.first << "," << xr.second << "], ["
						<< yr.first << "," << yr.second << "], ["
						<< zr.first << "," << zr.second << "])\n";
				;
    break;}
case 67:
#line 577 "quantum.y"
{
					//#?#currentSurface->Build();
					calcStages.pop();
					currentSurface = NULL;
					currentObject = NULL;
					WriteDone();
				;
    break;}
case 71:
#line 593 "quantum.y"
{
					currentSurface->SetThreshold(yyvsp[0].value);
				;
    break;}
case 72:
#line 597 "quantum.y"
{
					currentSurface->SetCyclic(yyvsp[0].realNum);
				;
    break;}
case 73:
#line 603 "quantum.y"
{
					WriteStatus("slice");
					currentSlice = new Slice();
					theVisualObjects.push_back(currentSlice);
					currentObject = currentSlice;
					calcStages.push(currentSlice);
				;
    break;}
case 74:
#line 611 "quantum.y"
{
					currentSlice->Build();
					currentSlice = NULL;
					currentObject = NULL;
					calcStages.pop();
					WriteDone();
				;
    break;}
case 78:
#line 626 "quantum.y"
{
					/*if($3->theRange.first < -1 || $3->theRange.second > 1
					 ||$3->value < -1 || $3->value > 1)
						throw DescError("Slice coordinate variable must be in [-1,1]");*/
					currentSlice->SetCoordValue(yyvsp[-2].integer,yyvsp[0].value);
					//$3->watchers.push_back(currentSlice->GetCoordValueWatcher());
				;
    break;}
case 79:
#line 634 "quantum.y"
{
					currentSlice->SetFramed(true);
				;
    break;}
case 80:
#line 640 "quantum.y"
{ yyval.integer = 0; ;
    break;}
case 81:
#line 642 "quantum.y"
{ yyval.integer = 1; ;
    break;}
case 82:
#line 644 "quantum.y"
{ yyval.integer = 2; ;
    break;}
case 83:
#line 648 "quantum.y"
{
					WriteStatus("vectors");
					currentVectors = new Vectors();
					theVisualObjects.push_back(currentVectors);
					currentObject = currentVectors;
					calcStages.push(currentVectors);
				;
    break;}
case 84:
#line 656 "quantum.y"
{
					currentVectors->SetData(yyvsp[0].field);
				;
    break;}
case 85:
#line 660 "quantum.y"
{
					calcStages.pop();
					currentVectors = NULL;
					currentObject = NULL;
					WriteDone();
				;
    break;}
case 89:
#line 673 "quantum.y"
{
					currentVectors->SetSecondaryColor(yyvsp[0].field);
				;
    break;}
case 90:
#line 677 "quantum.y"
{
					currentVectors->SetSphereColor(yyvsp[0].field);
				;
    break;}
case 91:
#line 681 "quantum.y"
{
					currentVectors->SetSphereSize(yyvsp[0].field);
				;
    break;}
case 92:
#line 685 "quantum.y"
{
					currentVectors->SetPoints(yyvsp[0].points);
				;
    break;}
case 93:
#line 691 "quantum.y"
{
					WriteStatus("flow");
					currentFlow = new Flow();
					theVisualObjects.push_back(currentFlow);
					currentObject = currentFlow;
					calcStages.push(currentFlow);
				;
    break;}
case 94:
#line 699 "quantum.y"
{
					currentFlow->SetData(yyvsp[0].field);
				;
    break;}
case 95:
#line 703 "quantum.y"
{
					calcStages.pop();
					currentFlow = NULL;
					currentObject = NULL;
					WriteDone();
				;
    break;}
case 99:
#line 715 "quantum.y"
{
					currentFlow->SetPoints(yyvsp[0].points);
				;
    break;}
case 100:
#line 719 "quantum.y"
{
					currentFlow->SetLength(yyvsp[0].value);
				;
    break;}
case 101:
#line 723 "quantum.y"
{
					currentFlow->SetStepsize(yyvsp[0].expr);
				;
    break;}
case 103:
#line 730 "quantum.y"
{
					yyval.points = autorelease(new GridPointList(yyvsp[-4].value,yyvsp[-2].value,yyvsp[0].value));
				;
    break;}
case 104:
#line 738 "quantum.y"
{
					yyval.points = autorelease(new ExplicitPointList());
					((ExplicitPointList*)yyval.points)->AddPoint(yyvsp[0].value);
				;
    break;}
case 105:
#line 743 "quantum.y"
{
					((ExplicitPointList*)yyvsp[-2].points)->AddPoint(yyvsp[0].value);
					yyval.points = yyvsp[-2].points;
				;
    break;}
case 106:
#line 750 "quantum.y"
{
					currentObject->SetColorFunction(yyvsp[0].field);
				;
    break;}
case 107:
#line 754 "quantum.y"
{
					currentObject->SetCutout(yyvsp[-5].range,yyvsp[-3].range,yyvsp[-1].range);
				;
    break;}
case 108:
#line 758 "quantum.y"
{
					calcStages.push(VisualObject::BSPMaterialUpdater);
				;
    break;}
case 109:
#line 762 "quantum.y"
{
					calcStages.pop();
					currentObject->SetTransparency(yyvsp[0].value);
				;
    break;}
case 110:
#line 767 "quantum.y"
{
					calcStages.push(VisualObject::BSPMaterialUpdater);
				;
    break;}
case 111:
#line 771 "quantum.y"
{
					calcStages.pop();
					currentObject->SetShininess(yyvsp[0].value);
				;
    break;}
case 112:
#line 778 "quantum.y"
{
					anim.sections.clear();
					animSection.vars.clear();
				;
    break;}
case 113:
#line 783 "quantum.y"
{
					//anim.GenerateAnimation();
					theAnimations.push_back(new Animator(anim));
					anim.sections.clear();
					
					std::cout << "NOTE:  The file contained information about an animation.\n";
					std::cout << "       Hit 'a' to generate the animation.\n";
				;
    break;}
case 116:
#line 799 "quantum.y"
{
					animSection.frames = yyvsp[0].integer;
					anim.sections.push_back(animSection);
					animSection.vars.clear();
				;
    break;}
case 122:
#line 818 "quantum.y"
{
					AnimatorVariableSpec animVar;
					animVar.var = GetRealVar(yyvsp[-4].str);
					animVar.range.first = yyvsp[-2].realNum;
					animVar.range.second = yyvsp[0].realNum;
					animVar.inclusive = false;
					animSection.vars.push_back(animVar);
				;
    break;}
case 123:
#line 828 "quantum.y"
{
					AnimatorVariableSpec animVar;
					animVar.var = GetRealVar(yyvsp[-5].str);
					animVar.range.first = yyvsp[-3].realNum;
					animVar.range.second = yyvsp[-1].realNum;
					animVar.inclusive = true;
					animSection.vars.push_back(animVar);
				;
    break;}
case 126:
#line 843 "quantum.y"
{
					GetRealVar(yyvsp[-3].str)->ChangeTo(yyvsp[-1].realNum);
				;
    break;}
case 127:
#line 856 "quantum.y"
{
					theSceneOptions.boxType = kBoxNone;
				;
    break;}
case 128:
#line 860 "quantum.y"
{
					theSceneOptions.boxType = kBoxColored;
				;
    break;}
case 129:
#line 864 "quantum.y"
{
					theSceneOptions.boxType = kBoxOneCustomColor;
					theSceneOptions.boxColorX = yyvsp[-1].real3;
				;
    break;}
case 130:
#line 869 "quantum.y"
{
					theSceneOptions.boxType = kBoxThreeCustomColors;
					theSceneOptions.boxColorX = yyvsp[-5].real3;
					theSceneOptions.boxColorY = yyvsp[-3].real3;
					theSceneOptions.boxColorZ = yyvsp[-1].real3;
				;
    break;}
case 131:
#line 876 "quantum.y"
{
					theSceneOptions.boxDivisions = yyvsp[-1].integer;
				;
    break;}
case 132:
#line 880 "quantum.y"
{
					theSceneOptions.xr =
						theSceneOptions.yr =
						theSceneOptions.zr = yyvsp[-1].range;
				;
    break;}
case 133:
#line 886 "quantum.y"
{
					theSceneOptions.xr = yyvsp[-6].range;
					theSceneOptions.yr = yyvsp[-4].range;
					theSceneOptions.zr = yyvsp[-2].range;
				;
    break;}
case 134:
#line 895 "quantum.y"
{
					vecN3 res = {yyvsp[-1].integer,yyvsp[-1].integer,yyvsp[-1].integer};
					ExpressionEvaluator::SetGlobalDefaultResolution(res);
				;
    break;}
case 135:
#line 903 "quantum.y"
{
					theSceneOptions.fogRange = yyvsp[-1].range;
				;
    break;}
case 136:
#line 910 "quantum.y"
{
					theSceneOptions.backgroundColor = yyvsp[-1].real3;
				;
    break;}
case 137:
#line 917 "quantum.y"
{
					yyval.real3.x = yyvsp[-5].realNum;
					yyval.real3.y = yyvsp[-3].realNum;
					yyval.real3.z = yyvsp[-1].realNum;
				;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 543 "/usr/share/bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;

 yyacceptlab:
  /* YYACCEPT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 0;

 yyabortlab:
  /* YYABORT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 1;
}
#line 924 "quantum.y"



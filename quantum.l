%{
#include "QuantumMath.h"
#include "QuantumDescription.h"
#include "quantum.tab.h"
#define YY_NEVER_INTERACTIVE 1
int yylex();

static int paren_star_counter = 0;

%}
%option noyywrap

%x	SLASH_STAR_COMMENT
%x	PAREN_STAR_COMMENT
%%

\/\/[^\r\n]*(\r(\n?)|\n)	curPosition.line++;
\#[^\r\n]*(\r(\n?)|\n)	curPosition.line++;
\/\*		BEGIN(SLASH_STAR_COMMENT);
<SLASH_STAR_COMMENT>\*\/	BEGIN(INITIAL);
<SLASH_STAR_COMMENT>(\r\n?|\n)	curPosition.line++;
<SLASH_STAR_COMMENT>.		/* swallow comment */
"(*"		BEGIN(PAREN_STAR_COMMENT);
<PAREN_STAR_COMMENT>"(*"	paren_star_counter++;
<PAREN_STAR_COMMENT>"*)"	{
				if(paren_star_counter > 0)
					paren_star_counter--;
				else
					BEGIN(INITIAL);
			}
<PAREN_STAR_COMMENT>(\r\n?|\n)	curPosition.line++;
<PAREN_STAR_COMMENT>.			/* swallow comment */

variable	return(kTok_variable);
field		return(kTok_field);
purge		return(kTok_purge);
isosurface	return(kTok_isosurface);
slice		return(kTok_slice);
vectors		return(kTok_vectors);
flow		return(kTok_flow);
window		return(kTok_window);
nobox		return(kTok_nobox);
colorbox	return(kTok_colorbox);
grid		return(kTok_grid);
world_size	return(kTok_world_size);
fog			return(kTok_fog);
background	return(kTok_background);

min			return(kTok_min);
max			return(kTok_max);
cx			return(kTok_cx);
cy			return(kTok_cy);
cz			return(kTok_cz);

resolution	return(kTok_resolution);
at			return(kTok_at);
cyclic		return(kTok_cyclic);
color		return(kTok_color);
cutout		return(kTok_cutout);
transparency	return(kTok_transparency);
framed		return(kTok_framed);
shininess	return(kTok_shininess);
secondary_color	return(kTok_secondary_color);
sphere_color	return(kTok_sphere_color);
sphere_radius	return(kTok_sphere_radius);
length		return(kTok_length);
stepsize	return(kTok_stepsize);

x			return(kTok_x);
y			return(kTok_y);
z			return(kTok_z);

i			return(kTok_i);

read		return(kTok_read);
abs			return(kTok_abs);
zoom		return(kTok_zoom);

scaled_to	return(kTok_scaled_to);
cut_to		return(kTok_cut_to);
filled_to	return(kTok_filled_to);

animate		return(kTok_animate);
from		return(kTok_from);
to			return(kTok_to);
and			return(kTok_and);
in			return(kTok_in);
frames		return(kTok_frames);
frame		return(kTok_frame);
inclusive	return(kTok_inclusive);

pi			{
				yylval.realNum = pi;
				return kTokReal;
			}
2pi			{
				yylval.realNum = pi * 2;
				return kTokReal;
			}

[a-z_A-Z][a-z0-9_A-Z]*	{
				yylval.str = yytext;
				
				identifierTypeMap::iterator p;
				p = definedIdentifierTypes.find(yylval.str);
				if(p == definedIdentifierTypes.end())
					return kTokIdentifier;
				return p->second;
			}

[0-9]+(\.[0-9]*)(e[\+\-]?[0-9]+)?			|
[0-9]+(\.[0-9]*)?(e[\+\-]?[0-9]+)			|
\.[0-9]+(e[\+\-]?[0-9]+)?					{
				yylval.realNum = atof(yytext);
				return kTokReal;
			}

[0-9]+		{
				yylval.integer = atoi(yytext);
				//cout << "string \"" << yytext << "\" converted to int "
				//	<< yylval.integer << endl;
				return kTokPosInteger;
			}
			
\"[^"]*\"	{ /*"\""*/
				yylval.str = yytext;
				yylval.str = yylval.str.substr(1,yylval.str.length()-2);
				
				return kTokString;
			}
			
":="		return(kTokAssign);

"("			|
")"			|
"["			|
"]"			|
"<"			|
">"			|
";"			|
","			|
"."			|
"="			|
"+"			|
"-"			|
"*"			|
"/"			return(yytext[0]);

[ \t]+		/* skip whitespace */
(\r\n?|\n)	curPosition.line++;

.			throw DescError(string("illegal token: ") + yytext[0]);

%%

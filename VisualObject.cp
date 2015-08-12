#include "VisualObject.h"
#include "ExpressionEvaluator.h"
#include "BSPTree.h"
#include "LockGL.h"

#include <iostream>
using namespace std;

BSPTree * VisualObject::BSP;
MaterialUpdater *VisualObject::BSPMaterialUpdater;

VisualObject::VisualObject()
	: colorFunction(NULL), transparencyExpr(NULL), shininessExpr(NULL),
		xCut(10,10), yCut(10,10), zCut(10,10) // ### empty Range?
{
	//colorFunctionWatcher.obj = this;
	//transparencyWatcher.obj = this;
	glObjectsDirty = false;
}

VisualObject::~VisualObject()
{
	if(colorFunction)
		colorFunction->DecRefCount();
	if(transparencyExpr) transparencyExpr->DecRefCount();
	if(shininessExpr) shininessExpr->DecRefCount();
}

void VisualObject::SetColorFunction(ExpressionEvaluator* color)
{
	if(colorFunction) colorFunction->DecRefCount();
	colorFunction = retain(color);
}

void VisualObject::SetCutout(Range xCutout, Range yCutout, Range zCutout)
{
	xCut = xCutout;
	yCut = yCutout;
	zCut = zCutout;
}

void VisualObject::SetTransparency(ValueEvaluator* trans)
{
	if(transparencyExpr) transparencyExpr->DecRefCount();
	transparencyExpr = retain(trans);
	BSP->DependOn(this);
}
void VisualObject::SetShininess(ValueEvaluator* shine)
{
	if(shininessExpr) shininessExpr->DecRefCount();
	shininessExpr = retain(shine);
}
/*
void VisualObject::ColorFunctionVariableWatcher::VariableChanged
			(RealVariable* var)
{
	obj->ColorChanged();
}*/

/*void VisualObject::TransparencyVariableWatcher::VariableChanged
			(RealVariable* var)
{
	obj->TransparencyChanged();
}


void VisualObject::TransparencyChanged()
{
	BSP->InvalidateTree();
}*/

void VisualObject::CalculateStage()
{
	Build();
}

bool VisualObject::GetObjectRanges(Range& xr, Range& yr, Range& zr)
{
	return false;
}

void VisualObject::OutputPOV(ostream& out)
{
	out << "#warning \"object type not supported\"" << endl;
}

bool VisualObject::GetRecommendedBSPResolution(vecN3& res)
{
	return false;
}

void VisualObject::SubmitToBSP()
{
	mainMaterial = BSP->CreateMaterial();
}

void VisualObject::UpdateMaterials()
{
	BSPTree::Material& m = BSP->GetMaterial(mainMaterial);
	if(transparencyExpr)
	{
		number tmp;
		transparencyExpr -> EvaluateReal(1,&tmp);
		m.alpha = 1-tmp;
	}
	else
		m.alpha = -1;
		
	if(shininessExpr)
	{
		number tmp;
		shininessExpr -> EvaluateReal(1,&tmp);
		m.shininess = tmp;
	}
	else
		m.shininess = -1;
}

void VisualObject::SetupShininess()
{
	number shine = -1;
	if(shininessExpr)
		shininessExpr -> EvaluateReal(1,&shine);
		
	if(shine < 0)
	{
		GLfloat mat_spec0[] = {0,0,0,1};
		glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,mat_spec0);
	}
	else
	{
		GLfloat shine1 = (GLfloat) shine;
		GLfloat mat_spec1[] = {1,1,1,1};
		glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,mat_spec1);
		glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,&shine1);
	}
}

void VisualObject::SetGLObjectsDirty()
{
	glObjectsDirty = true;
}

void VisualObject::BuildGLObjects()
{
	if(glObjectsDirty)
	{
		LockGL();
		BuildGLObjects1();
		UnlockGL();
	}
	glObjectsDirty = false;
}

void VisualObject::BuildGLObjects1()
{
}

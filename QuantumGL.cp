#include "QuantumConfig.h"
#include "CubicData.h"
#include "QuantumMath.h"
#include "DynArray.h"
#include "CubeInfo.h"
#include "IsoSurface.h"

#include "ReadEvaluator.h"
#include "AbsEvaluator.h"

#include "QuantumDescription.h"
#include "RealVariable.h"
#include "FieldFunctionUser.h"
#include "StandardFunctions.h"

#include "Animator.h"

#include "BSPTree.h"

#include <iostream>
#include <fstream>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <algorithm>

using namespace std;

#if QUANTUM_TARGET_MAC
#include <sioux.h>
#include <Profiler.h>
#endif

#include "ImageExporter.h"

#include "AutoreleasePool.h"

#include "QuantumFrontend.h"
#include "QuantumProgress.h"

#include "PredefinedVariables.h"

bool ExportImage(const char * name = NULL);

int sizeH = 640, sizeV = 480;

static bool glInited = false;

Range worldXRange, worldYRange, worldZRange;
number viewDiameter;

void UpdateVisualObjects()
{
	try
	{
		for(theVisualObjectsList::iterator p = theVisualObjects.begin();
			p != theVisualObjects.end(); ++p)
		{
			CheckAbort();
			(*p)->CalculateIfNecessary();
			CheckAbort();
			(*p)->BuildGLObjects();
		}
		CheckAbort();
		VisualObject::BSP -> CalculateIfNecessary();
		CheckAbort();
		VisualObject::BSPMaterialUpdater -> CalculateIfNecessary();
		CheckAbort();
	}
	catch(AbortException)
	{
	}
	catch(...)
	{
		cout << "*** UNEXPECTED EXCEPTION ***" << endl;
	}
	CalculationStage::AllUpdated();
}
void SetGLObjectsDirty()
{
	for(theVisualObjectsList::iterator p = theVisualObjects.begin();
		p != theVisualObjects.end(); ++p)
		(*p)->SetGLObjectsDirty();
	glInited = false;
}

GLfloat	yAngle, zAngle;
GLfloat lyAngle, lzAngle;
GLfloat fovAngle;
GLfloat camDistance;
GLfloat ambient;
GLfloat lineWidth;

RealVariable *camLatitudeVar, *camLongitudeVar;
RealVariable *lightLatitudeVar, *lightLongitudeVar;
RealVariable *fovyVar, *camDistanceVar;
RealVariable *ambientVar;
RealVariable *lineWidthVar;

class AngleVariableWatcher : public RealVariableWatcher
{
public:
	void VariableChanged(RealVariable* /*var*/)
	{
		yAngle = camLatitudeVar->value;
		zAngle = camLongitudeVar->value;
		lyAngle = lightLatitudeVar->value;
		lzAngle = lightLongitudeVar->value;
		fovAngle = fovyVar->value;
		camDistance = camDistanceVar->value;
		ambient = ambientVar->value;
		lineWidth = lineWidthVar->value;
	}
};

AngleVariableWatcher theAngleVarWatcher;

static void UpdateAngleVariables()
{
	camLatitudeVar->value = yAngle;
	camLongitudeVar->value = zAngle;
	lightLatitudeVar->value = lyAngle;
	lightLongitudeVar->value = lzAngle;
	fovyVar->value = fovAngle;
	camDistanceVar->value = camDistance;
	ambientVar->value = ambient;
	lineWidthVar->value = lineWidth;
}

void InitAngleVariables()
{
	yAngle = 40, zAngle = 50;
	lyAngle = 25, lzAngle = 45;
	fovAngle = 30;
	camDistance = -1;	// default value set in DetermineWorldSize below
	ambient = 0.1;
	lineWidth = 1;

	camLatitudeVar = new RealVariable();
	camLatitudeVar->constant = false;
	camLatitudeVar->name = "latitude";
	camLatitudeVar->theRange = Range(-89.999,89.999);
	camLatitudeVar->watchers.push_back(&theAngleVarWatcher);
	DefineRealVariable(camLatitudeVar);
	
	camLongitudeVar = new RealVariable();
	camLongitudeVar->constant = false;
	camLongitudeVar->name = "longitude";
	camLongitudeVar->theRange = fullRange;
	camLongitudeVar->watchers.push_back(&theAngleVarWatcher);
	DefineRealVariable(camLongitudeVar);

	lightLatitudeVar = new RealVariable();
	lightLatitudeVar->constant = false;
	lightLatitudeVar->name = "lightLatitude";
	lightLatitudeVar->theRange = Range(-89.999,89.999);
	lightLatitudeVar->watchers.push_back(&theAngleVarWatcher);
	DefineRealVariable(lightLatitudeVar);

	lightLongitudeVar = new RealVariable();
	lightLongitudeVar->constant = false;
	lightLongitudeVar->name = "lightLongitude";
	lightLongitudeVar->theRange = fullRange;
	lightLongitudeVar->watchers.push_back(&theAngleVarWatcher);
	DefineRealVariable(lightLongitudeVar);
	
	fovyVar = new RealVariable();
	fovyVar->constant = false;
	fovyVar->name = "fovy";
	fovyVar->theRange = fullRange;
	fovyVar->watchers.push_back(&theAngleVarWatcher);
	DefineRealVariable(fovyVar);
	
	camDistanceVar = new RealVariable();
	camDistanceVar->constant = false;
	camDistanceVar->name = "distance";
	camDistanceVar->theRange = fullRange;
	camDistanceVar->watchers.push_back(&theAngleVarWatcher);
	DefineRealVariable(camDistanceVar);

	ambientVar = new RealVariable();
	ambientVar->constant = false;
	ambientVar->name = "ambient";
	ambientVar->theRange = Range(0,1);
	ambientVar->watchers.push_back(&theAngleVarWatcher);
	DefineRealVariable(ambientVar);
	
	lineWidthVar = new RealVariable();
	lineWidthVar->constant = false;
	lineWidthVar->name = "lineWidth";
	lineWidthVar->theRange = Range(0,16);
	lineWidthVar->watchers.push_back(&theAngleVarWatcher);
	DefineRealVariable(lineWidthVar);

	UpdateAngleVariables();
}


void reshape(int w, int h)
{
	sizeH = w;
	sizeV = h;
	glViewport(0,0,w,h);
	PostRedisplay();
	cout << "The display area has been resized to " << w << 'x' << h << " pixels.\n";
}

static void RotateZ(GLfloat p[],GLfloat angle)
{
	GLfloat rad = angle/180 * 3.141593654;
	GLfloat s = sin(rad);
	GLfloat c = cos(rad);
	
	GLfloat nx = c*p[0] + -s*p[1];
	p[1] = s*p[0] + c*p[1];
	p[0] = nx;
}

static void RotateY(GLfloat p[],GLfloat angle)
{
	GLfloat rad = angle/180 * 3.141593654;
	GLfloat s = sin(rad);
	GLfloat c = cos(rad);
	
	GLfloat nx = c*p[0] + -s*p[2];
	p[2] = s*p[0] + c*p[2];
	p[0] = nx;
}


bool drawQuick = false;
bool drawQuickWhenDown = false;
int doCull = 0;

void Render()
{
}


void display(void)
{
}

void mouse(int button, int state, int x, int y)
{
}

void idle()
{
}


void DetermineWorldSize()
{
	worldXRange = theSceneOptions.xr;
	worldYRange = theSceneOptions.yr;
	worldZRange = theSceneOptions.zr;

	bool explicitRange = (worldXRange != fullRange);
		

	Range rx,ry,rz;
	for(theVisualObjectsList::iterator p = theVisualObjects.begin();
		p != theVisualObjects.end();++p)
	{
		if((*p)->GetObjectRanges(rx,ry,rz))
		{
			if(worldXRange == fullRange)
				worldXRange = rx, worldYRange = ry, worldZRange = rz;
			else
			{
				bool expanded = false;
				if(rx.first < worldXRange.first)
					worldXRange.first = rx.first, expanded = true;
				if(ry.first < worldYRange.first)
					worldYRange.first = ry.first, expanded = true;
				if(rz.first < worldZRange.first)
					worldZRange.first = rz.first, expanded = true;
				if(rx.second > worldXRange.second)
					worldXRange.second = rx.second, expanded = true;
				if(ry.second > worldYRange.second)
					worldYRange.second = ry.second, expanded = true;
				if(rz.second > worldZRange.second)
					worldZRange.second = rz.second, expanded = true;
				if(expanded && explicitRange)
				{
					cerr << "********* WARNING *********\n";
					cerr << "object doesn't fit in given world_size.\n";
				}
			}
		}
	}
	
	if(worldXRange == fullRange)
	{
		worldXRange = worldYRange = worldZRange = Range(-1,1);
	}
	viewDiameter = max(worldXRange.size(),max(worldYRange.size(),worldZRange.size()));

	if(camDistance == -1)
	{
		camDistance = viewDiameter * 7.0/2;
		UpdateAngleVariables();
	}
}

void SetNeedsReInit()
{
	glInited = false;
}

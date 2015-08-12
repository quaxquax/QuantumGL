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

#include GL_GL_H
#include GL_GLU_H
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
		//PostRedisplay();
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

//static GLdouble timePerFrame = 0;
//static GLint	nFrames = 0;

void Render()
{
	UpdateVisualObjects();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	gluPerspective(fovAngle,GLfloat(sizeH)/sizeV,camDistance - viewDiameter,camDistance + viewDiameter);
		
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	GLfloat	viewPoint[] = { camDistance,0,0,0};
	RotateY(viewPoint,yAngle);
	RotateZ(viewPoint,zAngle);
	
	gluLookAt(viewPoint[0],viewPoint[1],viewPoint[2],0,0,0,0,0,1);

	GLfloat lightPosition[] = { 1, 0, 0, 0.0 };
	RotateY(lightPosition,lyAngle);
	RotateZ(lightPosition,lzAngle);
	RotateY(lightPosition,yAngle);
	RotateZ(lightPosition,zAngle);

	GLfloat light_ambient[4];
	light_ambient[3] = 1;
	light_ambient[0] = light_ambient[1] = light_ambient[2] = ambient;
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);	
	
	glClearColor(	theSceneOptions.backgroundColor.x,
					theSceneOptions.backgroundColor.y,
					theSceneOptions.backgroundColor.z,
					1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glLineWidth(lineWidth);
#if 1
	if(theSceneOptions.boxDivisions)
	{
		glColor3f(1,0,0);
		glBegin(GL_LINES);		
		
			int nDiv = theSceneOptions.boxDivisions;
			for(int i=0;i<=nDiv;i++)
			{
				GLfloat coord1x = worldXRange.first + (worldXRange.size()/nDiv) * i;
				GLfloat coord1y = worldYRange.first + (worldYRange.size()/nDiv) * i;
				for(int j=0;j<=nDiv;j++)
				{
					if( (i==0 || i==nDiv) && (j==0 || j==nDiv) )
						continue;
					GLfloat coord2x = worldXRange.first + (worldXRange.size()/nDiv) * j;
					GLfloat coord2z = worldZRange.first + (worldZRange.size()/nDiv) * j;
					glVertex3f(coord2x,coord1y,worldZRange.first);
					glVertex3f(coord2x,coord1y,worldZRange.second);
					
					glVertex3f(worldXRange.first,coord1y,coord2z);
					glVertex3f(worldXRange.second,coord1y,coord2z);
					
					glVertex3f(coord1x,worldYRange.first,coord2z);
					glVertex3f(coord1x,worldYRange.second,coord2z);
				}
			}				
		glEnd();
	}
#endif
	if(theSceneOptions.boxType != kBoxNone || drawQuick)
	{	
		if(theSceneOptions.boxType == kBoxOneCustomColor)
			glColor3f(theSceneOptions.boxColorX.x,
						theSceneOptions.boxColorX.y,
						theSceneOptions.boxColorX.z);
		else
			glColor3f(1,1,1);
		glBegin(GL_LINES);
			if(theSceneOptions.boxType == kBoxColored)
				glColor3f(1,0,0);
			else if(theSceneOptions.boxType == kBoxThreeCustomColors)
				glColor3f(theSceneOptions.boxColorX.x,
						theSceneOptions.boxColorX.y,
						theSceneOptions.boxColorX.z);
				
			glVertex3f(worldXRange.first,worldYRange.first,worldZRange.first);
			glVertex3f(worldXRange.second,worldYRange.first,worldZRange.first);

			glVertex3f(worldXRange.first,worldYRange.first,worldZRange.second);
			glVertex3f(worldXRange.second,worldYRange.first,worldZRange.second);

			glVertex3f(worldXRange.first,worldYRange.second,worldZRange.second);
			glVertex3f(worldXRange.second,worldYRange.second,worldZRange.second);

			glVertex3f(worldXRange.first,worldYRange.second,worldZRange.first);
			glVertex3f(worldXRange.second,worldYRange.second,worldZRange.first);

			if(theSceneOptions.boxType == kBoxColored)
				glColor3f(0,1,0);
			else if(theSceneOptions.boxType == kBoxThreeCustomColors)
				glColor3f(theSceneOptions.boxColorY.x,
						theSceneOptions.boxColorY.y,
						theSceneOptions.boxColorY.z);
				
			glVertex3f(worldXRange.first,worldYRange.first,worldZRange.second);
			glVertex3f(worldXRange.first,worldYRange.second,worldZRange.second);

			glVertex3f(worldXRange.first,worldYRange.second,worldZRange.first);
			glVertex3f(worldXRange.first,worldYRange.first,worldZRange.first);

			glVertex3f(worldXRange.second,worldYRange.first,worldZRange.second);
			glVertex3f(worldXRange.second,worldYRange.second,worldZRange.second);

			glVertex3f(worldXRange.second,worldYRange.second,worldZRange.first);
			glVertex3f(worldXRange.second,worldYRange.first,worldZRange.first);
			
			if(theSceneOptions.boxType == kBoxColored)
				glColor3f(0,0,1);
			else if(theSceneOptions.boxType == kBoxThreeCustomColors)
				glColor3f(theSceneOptions.boxColorZ.x,
						theSceneOptions.boxColorZ.y,
						theSceneOptions.boxColorZ.z);
				
			glVertex3f(worldXRange.first,worldYRange.first,worldZRange.first);
			glVertex3f(worldXRange.first,worldYRange.first,worldZRange.second);

			glVertex3f(worldXRange.first,worldYRange.second,worldZRange.second);
			glVertex3f(worldXRange.first,worldYRange.second,worldZRange.first);

			glVertex3f(worldXRange.second,worldYRange.first,worldZRange.first);
			glVertex3f(worldXRange.second,worldYRange.first,worldZRange.second);

			glVertex3f(worldXRange.second,worldYRange.second,worldZRange.second);
			glVertex3f(worldXRange.second,worldYRange.second,worldZRange.first);
		glEnd();
	}
	
	if(!drawQuick)
	{
		for(theVisualObjectsList::iterator p = theVisualObjects.begin();
			p != theVisualObjects.end(); ++p)
		{
			(*p)->Draw(viewPoint,lightPosition);
		}
		VisualObject::BSP->Draw(viewPoint,lightPosition);
	}
}


void display(void)
{
	if(!glInited)
	{
		glInited = true;
		glEnable(GL_DEPTH_TEST);
		//GLfloat light_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
		//GLfloat light_ambient[] = { 0.3, 0.3, 0.3, 1.0 };
		GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
		GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };

		glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);	
		glEnable(GL_LIGHT0);
	
		glLightModelf (GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	
		/*GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
		GLfloat shininess = 50;
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &shininess);*/
	
	//	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	
		GLfloat fogColor[4] =
			{0.0, 0.0, 0.0, 1.0};
		glFogfv(GL_FOG_COLOR, fogColor);
		//glFogi(GL_FOG_MODE, GL_EXP);
		//glFogf(GL_FOG_DENSITY,0.02);
		/*glFogi(GL_FOG_MODE, GL_LINEAR);
		glFogf(GL_FOG_START, 5);
		glFogf(GL_FOG_END, 10);
	
		glEnable(GL_FOG);*/

		glPolygonOffset(2.0,2.0);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	}
//	GLint time = glutGet(GLUT_ELAPSED_TIME);
	
	if(theSceneOptions.fogRange == fullRange)
		glDisable(GL_FOG);
	else
	{
		glFogi(GL_FOG_MODE, GL_LINEAR);
		glFogf(GL_FOG_START, theSceneOptions.fogRange.first);
		glFogf(GL_FOG_END, theSceneOptions.fogRange.second);
		glEnable(GL_FOG);
	}

	if(doCull)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(doCull == 1 ? GL_FRONT : GL_BACK);
	}
	else
		glDisable(GL_CULL_FACE);
		
	Render();


	/*glEnable(GL_LIGHTING);
	glutSolidCube(0.5);
	glDisable(GL_LIGHTING);*/
	/*glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glBegin(GL_QUADS);
		glNormal3f(-1,0,0);
		glVertex3f(0,-1,-1);
		glVertex3f(0,1,-1);
		glVertex3f(0,1,1);
		glVertex3f(0,-1,1);
	glEnd();
	
	if(GLenum err = glGetError())
	{
		cout << "A error: " << err << endl;
	}
	*/
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_LIGHTING);

	SwapBuffers();

//	time = glutGet(GLUT_ELAPSED_TIME)-time;
	
//	timePerFrame += time;
//	nFrames++;
}

static int startX, startY;
static GLfloat startyangle, startzangle;
static GLfloat startlyangle, startlzangle;
static bool rotCam, rotLight;
static GLfloat deltaZ = 0;
static bool tracking = false;
//static GLint lastTime;

void motion(int x, int y)
{
	if(rotCam)
	{
		//GLint time = glutGet(GLUT_ELAPSED_TIME);

		GLfloat oldZ = zAngle;
		zAngle = startzangle - (x-startX);
		deltaZ = (zAngle-oldZ);
		yAngle = startyangle + (y-startY) * 0.5;
		if(yAngle <= -90) yAngle = -89;
		else if(yAngle >= 90) yAngle = 89;

		//lastTime = time;
	}
	if(rotLight)
	{
		lzAngle = startlzangle + (x-startX);
		lyAngle = startlyangle - (y-startY) * 0.5;
		if(lyAngle <= -90) lyAngle = -89;
		else if(lyAngle >= 90) lyAngle = 89;
	}
	UpdateAngleVariables();
	PostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
	if(state == kMBDown)
	{
		drawQuick = drawQuickWhenDown;
		tracking = true;
		startX = x, startY = y;
		startyangle = yAngle, startzangle = zAngle;
		startlyangle = lyAngle, startlzangle = lzAngle;
		rotCam = rotLight = false;
		//lastTime = glutGet(GLUT_ELAPSED_TIME);
		switch(button)
		{
			case kMBLeft:
					rotCam = true;
					break;
			case kMBRight:
					rotLight = true;
					break;
		}
	}
	else
	{
		drawQuick = false;
		PostRedisplay();
		tracking = false;
	}
}



void idle()
{
	//GLint time = glutGet(GLUT_ELAPSED_TIME);
	if(glInited)
	{
		if(abs(deltaZ) > 1 && !tracking)
		{
			zAngle += deltaZ > 0 ? 1 : -1;
			UpdateAngleVariables();
			PostRedisplay();
		}
	}
	//lastTime = time;
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

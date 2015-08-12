#include "QuantumConfig.h"
#include "QuantumFrontend.h"
#include "QuantumProgress.h"
#include "QuantumDescription.h"
#include "RealVariable.h"
#include "Animator.h"
#include "VisualObject.h"
#include "BSPTree.h"
#include "AutoreleasePool.h"
#include "FieldFunctionUser.h"
#include "StandardFunctions.h"
#include "CubeInfo.h"
#include "ImageExporter.h"

#if QUANTUM_TARGET_MAC
#include <SIOUX.h>
#endif

#include GL_GLUT_H
#include <iostream>
#include <fstream>

using namespace std;

definedRealVariablesMap::iterator currentVar;
bool showDrawing = false;

bool ShouldAbort()
{
	return false;
}

void CheckAbort() throw(AbortException)
{
}

void SetProgressMessage(string msg)
{
	cout << "Progress: " << msg << endl;
}

void StartDeterminateProgress()
{
}

void EndDeterminateProgress()
{
}

void Progress(double x)
{
}

void LockGL()
{
}

void UnlockGL()
{
}

bool ExportImage(const char * name)
{
	cout << "Rendering...\n";
	glDrawBuffer(showDrawing ? GL_FRONT : GL_BACK);
	Render();
	if(!ExportBackBuffer(sizeH,sizeV,name))
		return false;
	SwapBuffers();
	return true;
}

static void mouse1(int button, int state, int x, int y)
{
	switch(state)
	{
		case GLUT_DOWN: state = kMBDown; break;
		case GLUT_UP: state = kMBUp; break;
	}
	switch(button)
	{
		case GLUT_LEFT_BUTTON: button = kMBLeft; break;
		case GLUT_MIDDLE_BUTTON: button = kMBMiddle; break;
		case GLUT_RIGHT_BUTTON: button = kMBRight; break;
	}
	mouse(button,state,x,y);
}


static void special(int key,int,int)
{
	if(definedRealVariables.empty())
		return;
	switch(key)
	{
		case GLUT_KEY_LEFT:
							if(currentVar == definedRealVariables.begin())
								currentVar = definedRealVariables.end();
							--currentVar;
							cout << "Selected variable: " << currentVar->first << endl;
							break;	
		case GLUT_KEY_RIGHT:
							if(++currentVar == definedRealVariables.end())
								currentVar = definedRealVariables.begin();
							cout << "Selected variable: " << currentVar->first << endl;
							break;	
		case GLUT_KEY_UP:	currentVar->second->ChangeTo(currentVar->second->value + 0.02);
							cout << currentVar->first << " = " << currentVar->second->value << endl;
							break;
		case GLUT_KEY_DOWN:	currentVar->second->ChangeTo(currentVar->second->value - 0.02);
							cout << currentVar->first << " = " << currentVar->second->value << endl;
							break;
	}
	display();
}
static void ExportAnimations()
{
	while(!theAnimations.empty())
	{
		theAnimations.front()->GenerateAnimation();
		delete theAnimations.front();
		theAnimations.pop_front();
	}
}

static void ExportPOV()
{
	cout << "Exporting POVRay file...\n";
	ofstream out("QGLScene.pov");
	for(theVisualObjectsList::iterator p = theVisualObjects.begin();
			p != theVisualObjects.end();++p)
	{
		(*p)->OutputPOV(out);
	}
	out << "#include \"QGLCommon.inc\"\n";
	cout << "done.\n";
}

static void key(unsigned char key,int,int)
{
	switch(key)
	{
		/*case ' ':	cout << timePerFrame / nFrames << endl;
					timePerFrame = nFrames = 0;
					break;*/
		case '\r':
		case '\n':
					if(!definedRealVariables.empty())
					{
						cout << "Changing variable...\n";
						cout << currentVar->first << " = " << currentVar->second->value << endl;
						cout << currentVar->first << " := ";
						number newVal;
						cin >> newVal;
						currentVar->second->ChangeTo(newVal);
						glutPostRedisplay();
					}
					break;
/*		case 'p':	ProfilerDump("\p:profiles:Profile");
					break;*/
	/*	case 's':	showDrawing = !showDrawing;
					break;*/
		case 'x':	cout << "Rendering...\n";
					glDrawBuffer(showDrawing ? GL_FRONT : GL_BACK);
					Render();
					ExportBackBuffer(sizeH,sizeV);
					break;
		case 'a':	ExportAnimations();
					break;
		case 'f':	drawQuickWhenDown = !drawQuickWhenDown;
					break;
		case 'c':	doCull = (doCull+1)%3;
					glutPostRedisplay();
					break;
		case 't':	{
						GLint time = glutGet(GLUT_ELAPSED_TIME);
						cout << "starting speed test for 5 seconds...\n";
						GLint time2;
						GLint n = 0;
						while( (time2 = glutGet(GLUT_ELAPSED_TIME)) - time < 5000)
						{
							idle();
							display();
							n++;
						}
						cout << n << " frames in " << time2-time << "ms => "
							<< 1000*n/double(time2-time) << "fps\n";
					}
					break;
		case 'p':	ExportPOV();
					break;
					
		case 'q':	PrintReferenceCountingStatistics();
					ReleaseDescription();
					PrintReferenceCountingStatistics();
					exit(0);
					break;
	}
}


void ResizeDisplayTo(int w, int h)
{
	glutReshapeWindow(w,h);
}

void SwapBuffers()
{
	glutSwapBuffers();
}

void PostRedisplay()
{
	glutPostRedisplay();
}


#if QUANTUM_TARGET_MAC
void __assertion_failed(char const * a, char const * b, int i)
{
	cout << "Assertion Failed\n";
	cout << a << endl << b << endl << i << endl;
	throw string("assert");
}
#endif

#if QUANTUM_TARGET_MAC
int main()
#else
int main(int argc, char **argv)
#endif
{
#if QUANTUM_TARGET_MAC
	int argc = 1;
	char *progname = "qugel";
	char ** argv = &progname;
#endif
	glutInitWindowSize(sizeH,sizeV);

#if !QUANTUM_TARGET_MAC
	glutInit(&argc, argv);
#endif
#if QUANTUM_TARGET_MAC
	SIOUXSettings.setupmenus = false;
	SIOUXSettings.toppixel = 564;
	SIOUXSettings.rows = 15;
	SIOUXSettings.columns = 150;
	SIOUXSettings.leftpixel = 40;
	SIOUXSettings.showstatusline = true;
	SIOUXSettings.asktosaveonclose = false;
	SIOUXSettings.autocloseonquit = true;//false;
//	ProfilerInit(collectDetailed,bestTimeBase,200,200);
#endif

	string descriptionFile;
	if(argc == 2)
		descriptionFile = argv[1];
	else if(argc == 1)
	{
		cout << "Enter .qgl file to read: " << flush;
		char buffer[4096];
		cin.get(buffer,4095);
		descriptionFile = buffer;
	}
	else
	{
		cerr << "Usage: " << argv[0] << " [filename.qgl]\n";
		return 1;
	}

	cout << "Initializing OpenGL...\n";

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("QuantumGL");
	
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMotionFunc(motion);
	glutMouseFunc(mouse1);
	glutKeyboardFunc(key);
	glutSpecialFunc(special);
	glutIdleFunc(idle);
	
#if QUANTUM_TARGET_MAC
	SIOUXSettings.asktosaveonclose = false;
#endif
	
	cout << "GL_VENDOR: " << (const char*) glGetString(GL_VENDOR) << endl;
	cout << "GL_RENDERER: " << (const char*) glGetString(GL_RENDERER) << endl;
	cout << "GL_VERSION: " << (const char*) glGetString(GL_VERSION) << endl;
	cout << "GL_EXTENSIONS: " << (const char*) glGetString(GL_EXTENSIONS) << endl;
	
	AutoreleasePool::PushNewPool();
	VisualObject::BSP = new BSPTree();
	VisualObject::BSPMaterialUpdater = new MaterialUpdater();
	VisualObject::BSPMaterialUpdater -> DependOn(VisualObject::BSP);
	
	InitCubeInfo();
	RegisterFieldFunctions();
	RegisterStandardFunctions();
	InitAngleVariables();

	worldXRange = worldYRange = worldZRange = fullRange;

	/*worldXRange = Range(-1,1);
	worldYRange = Range(-1,1);
	worldZRange = Range(-0.5,0.5);*/

	cout << "reading description...\n";
	
	extern FILE* yyin;
	extern int yyparse();
	
	yyin = fopen(descriptionFile.c_str(),"r");
	if(yyin == NULL)
	{
		cout << "Description file \"" << descriptionFile << "\" could not be opened.\n";
		return 1;
	}

	curPosition.line = 1;
	
	volatile bool error = false;
	try
	{
		yyparse();
		PrintReferenceCountingStatistics();
		AutoreleasePool::FlushCurrentPool();
		PrintReferenceCountingStatistics();
		
		DetermineWorldSize();

		currentVar = definedRealVariables.begin();
	
		cout << "Entering main loop...\n";
		UpdateVisualObjects();
		glutMainLoop();
	}
	catch(DescError& e)
	{
		cout << "********* ERROR ********\n";
		cout << e.message << endl;
		cout << "line " << e.pos.line << endl;
		error = true;
		/*for(;;)
			;*/
	}
	catch(exception& e)
	{
		cout << "*********** EXCEPTION ***********\n";
		cout << e.what() << endl;
		error = true;

		/*for(;;)
			;*/
	}
	catch(...)
	{
		cout << "********* UNEXPECTED EXCEPTION ********\n";
		error = true;
		/*for(;;)
			;*/
	}
	PrintReferenceCountingStatistics();
	ReleaseDescription();
	PrintReferenceCountingStatistics();
#if QUANTUM_TARGET_MAC
	if(error)
	{
		cout << "press Command - . to exit\n";
		for(;;)
			cin.get();
	}
#endif
	return 0;
}


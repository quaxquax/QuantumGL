#define GLM_ENABLE_EXPERIMENTAL

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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <time.h>
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <stdio.h>
#include <cmath>
#include <iostream>
#include "shader_s.h"


#if QUANTUM_TARGET_MAC
#include <SIOUX.h>
#endif

//#include GL_GLUT_H
#include <iostream>
#include <fstream>

using namespace std;

// settings for display window
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

// Vertex shader source code 

const char* vertexShaderSource = R"(#version 300 es
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in float opa;
out vec4 ourColor;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 rotation;
void main(void)
{
 float alpha = 0.0f;
 mat4 rotationZ =  mat4(cos(alpha), sin(alpha), 0, 0,
                          -sin(alpha), cos(alpha), 0, 0,
                          0, 0, 1, 0,
                          0, 0, 0, 1);
    gl_Position = projection * view * model * rotation * vec4(aPos, 1.0);
    ourColor = vec4(aColor,opa);
}
)";

// Fragment shader source code 

const char* fragmentShaderSource = R"(#version 300 es
precision mediump float;       
in vec4 ourColor;           
 //"uniform vec4 ourColor;     
out vec4 FragColor;            
void main()                    
{                              
   FragColor = ourColor;    
}                             
)";

// Default scenario to render
 const char *example_text = 
   "fovy := 25;\n"
   "latitude := 20;\n"
   "longitude := 17;\n"
   "ambient := 1;\n"
   "colorbox;\n"
   "\n"
   "variable nradial := 1;\n"
   "variable lorbital := 3;\n"
   "variable mag := 2;\n"
   "\n"
   "field theData resolution 64 := CoulombFunction(nradial+lorbital+1, lorbital, mag, 1) cut_to [-50,50] scaled_to [-1,1];\n"
   "field absData := abs(theData);\n"
   "\n"
   "variable iso1 [0.0001,0.01] := 0.0013;\n"
   "variable iso2 := 0.0026;\n"
   "variable shine := 30;\n"
   "variable isotransp [0,1] := 0.4;\n"
   "\n"
   "isosurface absData at iso1 color argcolor(theData) cutout <[0,1],[0,1],[-1,1]> transparency isotransp shininess shine;\n"
   "isosurface absData at iso2 color argcolor(theData)  shininess shine;\n"
   "\n"
   "variable ypos [-1,1] := 0.0;\n"
   "variable zpos [-1,1] := -0.25;\n"
   "variable colormapR [0, 0.1] := 0.0026;\n"
   "variable slicetransp [0,1] := 0.2;\n"
   "\n"
   "slice y = ypos color complex_to_RGB(theData, colormapR) framed transparency slicetransp;\n"
   "slice z = zpos color complex_to_RGB(theData, colormapR) framed transparency slicetransp;\n";

// OpenGL context and rendering state
struct {
  GLuint shaderProgram;
  GLuint SolidVAO;
  GLuint SolidVBO;
  GLuint TransparentVAO;
  GLuint TransparentVBO;
  std::vector<float >  SolidVertices;
  std::vector<float > TransparentVertices;
  int numSolidVertices;
  int numTransparentVertices;
  glm::mat4 model;
  glm::mat4 view;
  glm::mat4 projection;
  glm::mat4 rotation;
} state;

int angle = 0;
int t0 = time(NULL);



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

bool ExportImage(const char * name)
{
	return true;
}

static void mouse1(int button, int state, int x, int y)
{
}


static void special(int key,int,int)
{
	if(definedRealVariables.empty())
		return;
	switch(key)
	{
		// case GLUT_KEY_LEFT:
		// 					if(currentVar == definedRealVariables.begin())
		// 						currentVar = definedRealVariables.end();
		// 					--currentVar;
		// 					cout << "Selected variable: " << currentVar->first << endl;
		// 					break;	
		// case GLUT_KEY_RIGHT:
		// 					if(++currentVar == definedRealVariables.end())
		// 						currentVar = definedRealVariables.begin();
		// 					cout << "Selected variable: " << currentVar->first << endl;
		// 					break;	
		// case GLUT_KEY_UP:	currentVar->second->ChangeTo(currentVar->second->value + 0.02);
		// 					cout << currentVar->first << " = " << currentVar->second->value << endl;
		// 					break;
		// case GLUT_KEY_DOWN:	currentVar->second->ChangeTo(currentVar->second->value - 0.02);
		// 					cout << currentVar->first << " = " << currentVar->second->value << endl;
		// 					break;
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

static void ExportVertices()
{
	cout << "Exporting Vertices Array ...\n";
	ofstream out("Vertices.txt");
	for(theVisualObjectsList::iterator p = theVisualObjects.begin();
			p != theVisualObjects.end();++p)
	{
	  (*p)->OutputVertices(std::cout);
	}
	cout << "done.\n";
}
static void ExportSolidVertices()
{
	cout << "Exporting Vertices Array ...\n";
	ofstream out("Vertices.txt");
	for(theVisualObjectsList::iterator p = theVisualObjects.begin();
			p != theVisualObjects.end();++p)
	{
	  (*p)->OutputSolidVertices(std::cout);
	}
	cout << "done.\n";
}
// static void AcquireVertices(float *SolidVertices, float *TransparentVertices, int SVsize, int TVsize)
static void AcquireVertices(std::vector<float> &Vertices, int Vsize)
{
  std::vector<float > TransparentVertices;
  std::vector<float > SolidVertices;

  // We will use this to keep track of the array offset index when iterating of the Visual Objects 
  int VObjectsOffsets[QUANTUM_VISUALOBJECT_TYPES];
  int VOCount=0;
  cout << "Current Vertice Array size: " << Vertices.size() << " elements.\n";
  cout << "Populating Vertices Array ...\n";
  
  ofstream out("Vertices.txt");
 
  for(theVisualObjectsList::iterator p = theVisualObjects.begin();
      p != theVisualObjects.end();++p)
    {
      VOCount++;
      // This won't work if we deal with more than one visual object
      // Need to keep track of the array offset index when iterating of the Visual Objects 
      (*p)->GetVertices(Vertices, Vsize);
    }

  std::cout << "Visual Objects count: " << VOCount << std::endl;
  std::cout << "Current Vertice Array size: " << Vertices.size() << " elements.\n";

  for (int i = 0; i < Vertices.size(); i += 21) {
    //    if (i + 20 < Vertices.size()) {
    if (Vertices[i + 6] != 1) {
      copy(Vertices.begin() + i, Vertices.begin() + i + 21, back_inserter(state.TransparentVertices));
    } else {
      copy(Vertices.begin() + i, Vertices.begin() + i + 21, back_inserter(state.SolidVertices));
    }
	// }
  }

  //   }
  cout << "done.\n";
}
static int NumberOfVerticeElements()
{
  int vCount=0;
  cout << "Counting all Vertices ...\n";
  for(theVisualObjectsList::iterator p = theVisualObjects.begin();
      p != theVisualObjects.end();++p)
    {
      vCount+=(*p)->CountVertices();
    }
  vCount=vCount;
  cout << "Number of Vertices: " << vCount << std::endl;
  cout << "done.\n";
  return vCount;
}
static int NumberOfSolidVertices()
{
  int vCount=0;
  cout << "Counting Solid Vertices ...\n";
  for(theVisualObjectsList::iterator p = theVisualObjects.begin();
      p != theVisualObjects.end();++p)
    {
      vCount+=(*p)->CountSolidVertices();
    }
  vCount=vCount;
  cout << "Number of Solid Vertices: " << vCount << std::endl;
  cout << "done.\n";
  return vCount;
}
static int NumberOfTransparentVertices()
{
  int vCount=0;
  cout << "Counting Transparent Vertices ...\n";
  for(theVisualObjectsList::iterator p = theVisualObjects.begin();
      p != theVisualObjects.end();++p)
    {
      vCount+=(*p)->CountTransparentVertices();
    }
  vCount=vCount;
  cout << "Number of Transparent Vertices: " << vCount << std::endl;
  cout << "done.\n";
  return vCount;
}

void mainLoop() {
    // Clear the screen
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

    // Use our shader program
    glUseProgram(state.shaderProgram);

    // retrieve the matrix uniform locations
    unsigned int modelLoc = glGetUniformLocation(state.shaderProgram, "model");
    unsigned int viewLoc  = glGetUniformLocation(state.shaderProgram, "view");
    unsigned int projLoc  = glGetUniformLocation(state.shaderProgram, "projection");
    unsigned int rotationLoc = glGetUniformLocation(state.shaderProgram, "rotation");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(state.model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(state.view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(state.projection));
    glUniformMatrix4fv(rotationLoc, 1, GL_FALSE, glm::value_ptr(state.rotation));

    int t = (time(NULL) - t0);
    
    //std::cout << "Time value: " << t << std::endl;

    // float greenValue = (sin(t) / 2.0f) + 0.5f;
    // int vertexColorLocation = glGetUniformLocation(state.shaderProgram, "ourColor");
    // Start rotation with a delay
    if (t>2)
      {
	unsigned int rotationLoc = glGetUniformLocation(state.shaderProgram, "rotation");
	glUseProgram(state.shaderProgram);
	glUniformMatrix4fv(rotationLoc, 1, GL_FALSE, glm::value_ptr(state.rotation));
	angle++;
	state.rotation = glm::rotate(glm::mat4(1.0f), glm::radians(angle/(20.0f)), glm::vec3(0.0, 0.0, 1.0));
      }

    // Draw the triangle
    glBindVertexArray(state.SolidVAO);
    //glDrawArrays(GL_POINTS, 0, 3);
    //glDrawArrays(GL_TRIANGLES, 0, 3);
    //glDrawArrays(GL_TRIANGLES, 0, 6);
    // This fills the triangles if we work with an EBO
    //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    //Wirframe mode can't be switched on with glPolygonMode in WebGL, but this will work instead
    //glDrawElements(GL_LINE_LOOP, 6, GL_UNSIGNED_INT, 0);

    glDrawArrays(GL_TRIANGLES, 0, state.numSolidVertices);

    glBindVertexArray(state.TransparentVAO);
    glDrawArrays(GL_TRIANGLES, 0, state.numTransparentVertices);

    
    // glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
}
 

static void key(unsigned char key,int,int)
{
	switch(key)
	{

		case 'v':	ExportVertices();
					break;
		case 'o':	ExportSolidVertices();
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
}

void SwapBuffers()
{
}

void PostRedisplay()
{
}


#if QUANTUM_TARGET_MAC
void __assertion_failed(char const * a, char const * b, int i)
{
	cout << "Assertion Failed\n";
	cout << a << endl << b << endl << i << endl;
	throw string("assert");
}
#endif


EM_BOOL resizeCanvas(int eventType, const EmscriptenUiEvent* uiEvent, void* userData) {
    int width, height;
    emscripten_get_canvas_element_size("#canvas", &width, &height);
    glViewport(0, 0, width, height);
    return true;
}

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


#if QUANTUM_TARGET_MAC
	SIOUXSettings.setupmenus = false;
	SIOUXSettings.toppixel = 564;
	SIOUXSettings.rows = 15;
	SIOUXSettings.columns = 150;
	SIOUXSettings.leftpixel = 40;
	SIOUXSettings.showstatusline = true;
	SIOUXSettings.asktosaveonclose = false;
	SIOUXSettings.autocloseonquit = true;//false;
#endif

	string descriptionFile;
	if(argc == 2)
		descriptionFile = argv[1];
	else if(argc == 1)
	{
	            // Write the text to the named file
	  FILE *file = fopen("TempTextFile.qgl", "w");
	  if (!file) {
	    perror("Failed to create file");
	    exit(EXIT_FAILURE);
	  }
	  fputs(example_text, file);
	  fclose(file);
	  // Original code to ask for filename

		// cout << "Enter .qgl file to read: " << flush;
		// char buffer[4096];
		// cin.get(buffer,4095);
		// descriptionFile = buffer;
	  descriptionFile = "TempTextFile.qgl";
	}
	else
	{
		cerr << "Usage: " << argv[0] << " [filename.qgl]\n";
		return 1;
	}

	cout << "Initializing OpenGL...\n";

#if QUANTUM_TARGET_MAC
	SIOUXSettings.asktosaveonclose = false;
#endif
		
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
	  bool diagnostic=true;

	  // Initialize WebGL context
	  EmscriptenWebGLContextAttributes attrs;
	  emscripten_webgl_init_context_attributes(&attrs);
	  attrs.majorVersion = 3;
	  attrs.minorVersion = 0;
	  EMSCRIPTEN_WEBGL_CONTEXT_HANDLE context = emscripten_webgl_create_context("#canvas", &attrs);
	  emscripten_webgl_make_context_current(context);
	  
	  // Add canvas resize listener
	  emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, true, resizeCanvas);

	  glEnable(GL_DEPTH_TEST);
	  glEnable(GL_BLEND);
	  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	  
	  yyparse();
	  PrintReferenceCountingStatistics();
	  AutoreleasePool::FlushCurrentPool();
	  PrintReferenceCountingStatistics();
	  
	  DetermineWorldSize();
	  
	  currentVar = definedRealVariables.begin();
	  
	  cout << "Entering main loop...\n";
	  UpdateVisualObjects();
	  int numV;
	  numV=NumberOfVerticeElements();
	  //state.numSolidVertices=NumberOfSolidVertices();
	  //state.numTransparentVertices=NumberOfTransparentVertices();

	  //std::vector<float > allVertices(numV);
	  std::vector<float > allVertices;
	    //float mySolidVertices[7*state.numSolidVertices];
	    //float myTransparentVertices[7*state.numTransparentVertices];
	    //float allVertices[numV];

	  // std::cout << "Received number of vertices: " << numV << std::endl;	  
	  
	  AcquireVertices(allVertices,numV);
	  if (diagnostic) {
	    cout << "Some semi-transparent vertices: \n"<< std::endl;;
	    for(int i=0;i<100;i++)
	      {
		cout << state.TransparentVertices[i] << ',' ;
		if (!((i+1) % 7))
		  cout << '\n';    
	      }
	    cout << "Next Array .. \n"<< std::endl;;
	    cout << "Some solid vertices: \n"<< std::endl;;    
	    for(int i=0;i<100;i++)
	      {
		cout << state.SolidVertices[i] << ',' ;
		if (!((i+1) % 7))
		  cout << '\n';    
	      }
	  }
	  cout << "\nNumber of solid vertice elements: " << state.TransparentVertices.size() << ",\n";
	  cout << "\nNumber of semi-transparent vertix elements: " << state.SolidVertices.size() << ",\n";

	  state.numSolidVertices = state.SolidVertices.size() / 7;
	  state.numTransparentVertices = state.TransparentVertices.size() / 7;

	  
	  Shader myShader(vertexShaderSource, fragmentShaderSource);

	  // The order here is critically important for emscripten
	  // The VAO glBindVertexArray has to happen before the binding of attribute and element buffers 
	  // Create and bind Vertex Array Object for Solid Vertices
	  glGenVertexArrays(1, &state.SolidVAO);
	  glBindVertexArray(state.SolidVAO);
	  
	  // Create and bind Vertex Buffer Object
	  glGenBuffers(1, &state.SolidVBO);
	  
	  glBindBuffer(GL_ARRAY_BUFFER, state.SolidVBO);    
	  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * state.SolidVertices.size(), &state.SolidVertices[0], GL_STATIC_DRAW);
	  
	  // position attribute
	  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
	  glEnableVertexAttribArray(0);
	  // color attribute
	  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3* sizeof(float)));
	  glEnableVertexAttribArray(1);
	  // transperancy attribute
	  glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(6* sizeof(float)));
	  glEnableVertexAttribArray(2);   
	  
	  // Create and bind Vertex Array Object for semi-transparent Vertices
	  glGenVertexArrays(1, &state.TransparentVAO);
	  glBindVertexArray(state.TransparentVAO);
	  
	  // Create and bind Vertex Buffer Object
	  glGenBuffers(1, &state.TransparentVBO);
    
	  glBindBuffer(GL_ARRAY_BUFFER, state.TransparentVBO);    
	  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * state.TransparentVertices.size(), &state.TransparentVertices.front(), GL_STATIC_DRAW);

	  // position attribute
	  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
	  glEnableVertexAttribArray(0);
	  // color attribute
	  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3* sizeof(float)));
	  glEnableVertexAttribArray(1);
	  // transperancy attribute
	  glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(6* sizeof(float)));
	  glEnableVertexAttribArray(2);   	  

	  // create transformations
	  state.model         = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
	  state.view          = glm::mat4(1.0f);
	  state.projection    = glm::mat4(1.0f);
	  state.model = glm::rotate(state.model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	  state.view  = glm::translate(state.view, glm::vec3(0.0f, 0.0f, -3.0f));
	  state.projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	  
    
	  state.rotation = glm::mat4(1.0f);

	  // Set rendering loop
    
	  state.shaderProgram = myShader.ID;

	  std::cout << "Matrix: " << glm::to_string(state.rotation) << std::endl;

	  
	  // Place to insert emscripten loop?
	  emscripten_set_main_loop(mainLoop, 0, true);

		
	}
	catch(DescError& e)
	{
		cout << "********* ERROR ********\n";
		cout << e.message << endl;
		cout << "line " << e.pos.line << endl;
		error = true;
	}
	catch(exception& e)
	{
		cout << "*********** EXCEPTION ***********\n";
		cout << e.what() << endl;
		error = true;
	}
	catch(...)
	{
		cout << "********* UNEXPECTED EXCEPTION ********\n";
		error = true;
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


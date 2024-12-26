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

#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

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


// Global window handle
GLFWwindow* window = nullptr;
definedRealVariablesMap::iterator currentVar;
bool showDrawing = false;

// Error callback
void error_callback(int error, const char* description) {
    cerr << "GLFW Error " << error << ": " << description << endl;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    reshape(width, height);
}

// Add this global variable to track mouse button state
bool mouseButtonPressed = false;
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    
    int state;
    switch(action) {
        case GLFW_PRESS: 
            state = kMBDown; 
            mouseButtonPressed = true;
            break;
        case GLFW_RELEASE: 
            state = kMBUp; 
            mouseButtonPressed = false;
            break;
        default: return;
    }
    
    int translatedButton;
    switch(button) {
        case GLFW_MOUSE_BUTTON_LEFT: translatedButton = kMBLeft; break;
        case GLFW_MOUSE_BUTTON_MIDDLE: translatedButton = kMBMiddle; break;
        case GLFW_MOUSE_BUTTON_RIGHT: translatedButton = kMBRight; break;
        default: return;
    }
    
    mouse(translatedButton, state, static_cast<int>(xpos), static_cast<int>(ypos));
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    // Only process motion when a button is pressed
    if (mouseButtonPressed) {
        motion(static_cast<int>(xpos), static_cast<int>(ypos));
    }
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action != GLFW_PRESS) return;
    
    if (key >= GLFW_KEY_A && key <= GLFW_KEY_Z) {
        key = 'a' + (key - GLFW_KEY_A);
    }
    
    switch(key) {
        case GLFW_KEY_ENTER:
            if(!definedRealVariables.empty()) {
                cout << "Changing variable...\n";
                cout << currentVar->first << " = " << currentVar->second->value << endl;
                cout << currentVar->first << " := ";
                number newVal;
                cin >> newVal;
                currentVar->second->ChangeTo(newVal);
                display();
            }
            break;
        case GLFW_KEY_X:
            cout << "Rendering...\n";
            glDrawBuffer(showDrawing ? GL_FRONT : GL_BACK);
            Render();
            ExportBackBuffer(sizeH, sizeV);
            break;
        // ... rest of your key handlers ...
    }
}

void special_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action != GLFW_PRESS && action != GLFW_REPEAT) return;
    
    if(definedRealVariables.empty())
        return;
        
    switch(key) {
        case GLFW_KEY_LEFT:
            if(currentVar == definedRealVariables.begin())
                currentVar = definedRealVariables.end();
            --currentVar;
            cout << "Selected variable: " << currentVar->first << endl;
            break;
        case GLFW_KEY_RIGHT:
            if(++currentVar == definedRealVariables.end())
                currentVar = definedRealVariables.begin();
            cout << "Selected variable: " << currentVar->first << endl;
            break;
        case GLFW_KEY_UP:
            currentVar->second->ChangeTo(currentVar->second->value + 0.02);
            cout << currentVar->first << " = " << currentVar->second->value << endl;
            break;
        case GLFW_KEY_DOWN:
            currentVar->second->ChangeTo(currentVar->second->value - 0.02);
            cout << currentVar->first << " = " << currentVar->second->value << endl;
            break;
    }
    display();
}

void ResizeDisplayTo(int w, int h) {
    glfwSetWindowSize(window, w, h);
}

void SwapBuffers() {
    glFlush();
    glFinish();
}

void PostRedisplay() {
    display();
}

int main(int argc, char **argv) {
    if (!glfwInit()) {
        cerr << "Failed to initialize GLFW" << endl;
        return -1;
    }
    // Verify GLFW version
    int major, minor, rev;
    glfwGetVersion(&major, &minor, &rev);
    printf("GLFW Version: %d.%d.%d\n", major, minor, rev);
    
    glfwSetErrorCallback(error_callback);

    // Window hints for OpenGL context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);  // Single buffered

    window = glfwCreateWindow(sizeH, sizeV, "QuantumGL", nullptr, nullptr);
    if (!window) {
        cerr << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Set callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetKeyCallback(window, key_callback);
    
    string descriptionFile;
    if(argc == 2)
        descriptionFile = argv[1];
    else if(argc == 1) {
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
    else {
        cerr << "Usage: " << argv[0] << " [filename.qgl]\n";
        return 1;
    }

    cout << "Initializing OpenGL...\n";
    
    cout << "GL_VENDOR: " << (const char*) glGetString(GL_VENDOR) << endl;
    cout << "GL_RENDERER: " << (const char*) glGetString(GL_RENDERER) << endl;
    cout << "GL_VERSION: " << (const char*) glGetString(GL_VERSION) << endl;
    cout << "GL_EXTENSIONS: " << (const char*) glGetString(GL_EXTENSIONS) << endl;

    AutoreleasePool::PushNewPool();
    VisualObject::BSP = new BSPTree();
    VisualObject::BSPMaterialUpdater = new MaterialUpdater();
    VisualObject::BSPMaterialUpdater->DependOn(VisualObject::BSP);
    
    InitCubeInfo();
    RegisterFieldFunctions();
    RegisterStandardFunctions();
    InitAngleVariables();

    worldXRange = worldYRange = worldZRange = fullRange;

    cout << "reading description...\n";
    
    extern FILE* yyin;
    extern int yyparse();
    
    yyin = fopen(descriptionFile.c_str(),"r");
    if(yyin == NULL) {
        cout << "Description file \"" << descriptionFile << "\" could not be opened.\n";
        return 1;
    }

    curPosition.line = 1;
    
    try {
        yyparse();
        PrintReferenceCountingStatistics();
        AutoreleasePool::FlushCurrentPool();
        PrintReferenceCountingStatistics();
        
        DetermineWorldSize();
        currentVar = definedRealVariables.begin();
    
        cout << "Entering main loop...\n";
        UpdateVisualObjects();

        // Main loop
        while (!glfwWindowShouldClose(window)) {
            display();  // Your render function
            idle();    // Your idle function
            
            glfwPollEvents();
        }
        
        PrintReferenceCountingStatistics();
        ReleaseDescription();
        PrintReferenceCountingStatistics();
        
        glfwDestroyWindow(window);
        glfwTerminate();
    }
    catch(DescError& e) {
        cout << "********* ERROR ********\n";
        cout << e.message << endl;
        cout << "line " << e.pos.line << endl;
        glfwTerminate();
        return 1;
    }
    catch(exception& e) {
        cout << "*********** EXCEPTION ***********\n";
        cout << e.what() << endl;
        glfwTerminate();
        return 1;
    }
    catch(...) {
        cout << "********* UNEXPECTED EXCEPTION ********\n";
        glfwTerminate();
        return 1;
    }

    return 0;
}



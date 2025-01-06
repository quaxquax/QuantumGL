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
#include "OBJExporter.h"

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

// const char *example_text = 
//   "fovy := 25;\n"
//   "latitude := 20;\n"
//   "longitude := 17;\n"
//   "variable cn := 1;\n"
//   "variable cl := 1;\n"
//   "variable cm := 0;\n"
//   "variable a [0,1] := 0;\n"
//   "field theData resolution 64 := sqrt(1. - a*a)*CoulombFunctionT(1,0,0,1,100*a) + a*CoulombFunctionT(2, 1,1,1,100*a) cut_to [-12,12] scaled_to [-1,1];\n"
//   "field absData := abs(theData);\n"
//   "variable iso1:=0.006;\n"
//   "variable iso2:=0.03;\n"
//   "isosurface absData at iso1 color argcolor(theData) transparency 0.7 shininess 50;\n"
//   "isosurface absData at iso2 color argcolor(theData) shininess 50;\n";

// Global window handle
GLFWwindow* window = nullptr;
definedRealVariablesMap::iterator currentVar;
bool showDrawing = false;
VisualObject* currentObject = nullptr;
VisualObject* lastCreatedObject = nullptr;  // Track the last created object

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
    
    // Parse command line arguments
    string descriptionFile;
    string objOutputFile;
    bool exportToObj = false;
    
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "--export-obj") {
            if (i + 1 < argc) {
                objOutputFile = argv[++i];
                exportToObj = true;
            } else {
                cerr << "Error: --export-obj requires an output file path\n";
                return 1;
            }
        } else if (descriptionFile.empty()) {
            descriptionFile = arg;
        } else {
            cerr << "Usage: " << argv[0] << " [filename.qgl] [--export-obj output.obj]\n";
            return 1;
        }
    }
    
    if (descriptionFile.empty()) {
        // Write the text to the named file
        FILE *file = fopen("TempTextFile.qgl", "w");
        if (!file) {
            perror("Failed to create file");
            exit(EXIT_FAILURE);
        }
        fputs(example_text, file);
        fclose(file);
        descriptionFile = "TempTextFile.qgl";
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
    
    cout << "Initializing OpenGL...\n";

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

        // If export to OBJ was requested, do it and exit
        if (exportToObj) {
            if (!theVisualObjects.empty()) {
                // Find the last object with mesh data (usually an IsoSurface)
                VisualObject* lastMeshObject = nullptr;
                for (auto it = theVisualObjects.rbegin(); it != theVisualObjects.rend(); ++it) {
                    if ((*it)->HasMeshData()) {
                        lastMeshObject = *it;
                        break;
                    }
                }

                if (lastMeshObject) {
                    OBJExporter exporter;
                    if (exporter.ExportToOBJ(*lastMeshObject, objOutputFile.c_str())) {
                        cout << "Successfully exported to " << objOutputFile << "\n";
                        glfwTerminate();
                        return 0;
                    } else {
                        cerr << "Failed to export to OBJ\n";
                        glfwTerminate();
                        return 1;
                    }
                } else {
                    cerr << "No objects with mesh data found to export\n";
                    glfwTerminate();
                    return 1;
                }
            } else {
                cerr << "No objects to export\n";
                glfwTerminate();
                return 1;
            }
        }

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

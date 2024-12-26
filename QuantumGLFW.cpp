#include "QuantumConfig.h"
#include "QuantumFrontend.h"
#include "QuantumProgress.h"
#include "ImageExporter.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <mutex>

using namespace std;

// External declarations for window size
extern int sizeH;
extern int sizeV;
extern void Render();

// Mutex for OpenGL context locking
static std::mutex glMutex;

// Function implementations
bool ShouldAbort()
{
    return false;
}

void CheckAbort() throw(AbortException)
{
    // Implementation remains empty as in original
}

void SetProgressMessage(string msg)
{
    cout << "Progress: " << msg << endl;
}

void StartDeterminateProgress()
{
    // Implementation can be empty if no progress bar UI is needed
}

void EndDeterminateProgress()
{
    // Implementation can be empty if no progress bar UI is needed
}

void Progress(double x)
{
    // Optional: Could print progress percentage
    // cout << "Progress: " << (x * 100) << "%" << endl;
}

void LockGL()
{
    glMutex.lock();
}

void UnlockGL()
{
    glMutex.unlock();
}

bool ExportImage(const char* name)
{
    cout << "Rendering...\n";
    glDrawBuffer(GL_FRONT);
    Render();
    if(!ExportBackBuffer(sizeH, sizeV, name))
        return false;
    glFlush();
    glFinish();
    return true;
}

#include "QuantumMath.h"

void UpdateVisualObjects();
void SetGLObjectsDirty();
void InitAngleVariables();

void reshape(int w, int h);
void Render(void);
void display(void);
void motion(int x, int y);

enum
{
	kMBUp,
	kMBDown
};

enum
{
	kMBLeft, kMBMiddle, kMBRight
};

void mouse(int button, int state, int x, int y);
void idle();
void DetermineWorldSize();

//extern bool showDrawing;
extern int sizeH, sizeV;
extern Range worldXRange, worldYRange, worldZRange;
extern number viewDiameter;

extern bool drawQuick;
extern bool drawQuickWhenDown;

extern int doCull; //###?
// ----

void ResizeDisplayTo(int w, int h);
void SwapBuffers();
void PostRedisplay();


void SetNeedsReInit();

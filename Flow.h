#ifndef _H_FLOW
#define _H_FLOW

#include "VisualObject.h"
#include "RealVariable.h"
#include GL_GL_H

class PointList;

class Flow : public VisualObject
{
	PointList			*thePointList;
	ExpressionEvaluator	*theData;
	ValueEvaluator		*theLength;
	CommonEvaluator		*theStepsize;
	
	class Line
	{
		Flow			*flow;
		vector<vecR3>	points;
		vector<vecR3>	colors;
		vector<vecR3>	tangents;
	public:
		void		Build(Flow *flow, vecR3 from);
		void		Draw(GLfloat *camera = NULL, GLfloat *light = NULL);
	};

	friend class Flow::Line;
	vector<Line> lines;
	
	GLuint			lightingTextureObject;	

	class AmbientWatcher : public ::RealVariableWatcher
	{
		Flow		*theFlow;
	public:
					AmbientWatcher(Flow *f) : theFlow(f) {}
			void	VariableChanged(RealVariable* var);
	};
	AmbientWatcher	ambientWatcher;
	
public:
					Flow();
					~Flow();

			void	SetPoints(PointList *points);
			void	SetData(ExpressionEvaluator *function);
			void	SetLength(ValueEvaluator *length);
			void	SetStepsize(CommonEvaluator *stepsize);

	virtual void	Draw(GLfloat camera[3], GLfloat light[3]);
	virtual void	Build();
	virtual void	BuildGLObjects1();
};

#endif

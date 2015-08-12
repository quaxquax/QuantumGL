#ifndef _H_VECTORS
#define _H_VECTORS

#include "VisualObject.h"

class PointList;

class Vectors : public VisualObject
{
	PointList			*thePointList;
	ExpressionEvaluator	*theData;

	ExpressionEvaluator	*secondaryColor;
	ExpressionEvaluator *sphereColor;
	ExpressionEvaluator	*sphereSize;
public:
					Vectors();
					~Vectors();

			void	SetPoints(PointList *points);
			void	SetData(ExpressionEvaluator *function);
			void	SetSecondaryColor(ExpressionEvaluator *f);
			void	SetSphereColor(ExpressionEvaluator *f);
			void	SetSphereSize(ExpressionEvaluator *f);

	virtual void	Draw(GLfloat camera[3], GLfloat light[3]);
	virtual void	Build();
};

#endif

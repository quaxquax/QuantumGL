#ifndef _H_SLICE
#define _H_SLICE

#include "VisualObject.h"
#include GL_GL_H

class Slice : public VisualObject
{
	int	plane;
	ValueEvaluator	*coordValueExpr;

	int		displayListIndex;
	bool	displayListDirty;

	bool	useTextures;
	bool	frameSlice;

	GLuint		textureObject;

	Range	xRange, yRange, zRange;
public:
			Slice();
			~Slice();

	void	SetCoordValue(int plane, ValueEvaluator* coord);
	void	SetFramed(bool f) { frameSlice = f; }

	void	Draw(GLfloat camera[3], GLfloat light[3]);
	void	Build();
	void	BuildGLObjects1();
	void	SetGLObjectsDirty();
	
	void	SubmitToBSP();
	void	UpdateMaterials();

	virtual bool	GetObjectRanges(Range& xr, Range& yr, Range& zr);
};

#endif

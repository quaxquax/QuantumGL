#ifndef _H_VISUAL_OBJECT
#define _H_VISUAL_OBJECT

#include "QuantumConfig.h"
#include "QuantumMath.h"
#include "CalculationStage.h"

#include GL_GL_H

class ExpressionEvaluator;
class ValueEvaluator;

class BSPTree;
class MaterialUpdater;

class VisualObject : public CalculationStage
{
protected:
	ExpressionEvaluator	*colorFunction;
	ValueEvaluator	*transparencyExpr;
	ValueEvaluator	*shininessExpr;

	int				mainMaterial;
	bool			glObjectsDirty;
	
	Range	xCut, yCut, zCut;	

	virtual void	CalculateStage();
	
			void	SetupShininess();
public:
					VisualObject();
	virtual			~VisualObject();

	virtual void	SetColorFunction(ExpressionEvaluator *color);
	virtual void	SetCutout(Range xCutout, Range yCutout, Range zCutout);
	virtual void	SetTransparency(ValueEvaluator *trans);
	virtual void	SetShininess(ValueEvaluator *shine);

	/*RealVariableWatcher	*GetColorFunctionWatcher()
			{	return &colorFunctionWatcher;	}*/
	/*RealVariableWatcher	*GetTransparencyWatcher()
			{	return &transparencyWatcher;	}*/


	virtual void	Draw(GLfloat camera[3], GLfloat light[3]) = 0;
	virtual void	Build() = 0;
	
	virtual	void	SetGLObjectsDirty();
			void	BuildGLObjects();
	virtual void	BuildGLObjects1();
	
	virtual void	SubmitToBSP();
	virtual void	OutputPOV(ostream& out);

	static BSPTree	*BSP;
	static MaterialUpdater	*BSPMaterialUpdater;

	virtual bool	GetObjectRanges(Range& xr, Range& yr, Range& zr);

	virtual bool	GetRecommendedBSPResolution(vecN3& res);
	
	virtual void	UpdateMaterials();
};

#endif

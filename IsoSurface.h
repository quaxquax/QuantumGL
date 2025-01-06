#ifndef _H_ISOSURFACE
#define _H_ISOSURFACE

#include "VisualObject.h"
//#include "QuantumConfig.h"
//#include "QuantumMath.h"
#include "CubicData.h"
#include "DynArray.h"
#include GL_GL_H

class ExpressionEvaluator;
class ValueEvaluator;

class IsoSurface : public VisualObject
{
	struct Critical
	{
		vecN3	cube;
		GLubyte	type;
	};	
	
	number	threshold;
	
	DynArray<vecR3> theVertices;
	DynArray<vecR3> theNormals;
	DynArray<vecR3> theColors;
	DynArray<GLint> theIndices;
	DynArray<vecN3>	theCubes;
	int		displayListIndex;
	bool	displayListDirty;
	
	ExpressionEvaluator	*dataFunction;
	GeneralCubicData	*genData;	
	CubicData<number>	*data;
	
	vecR3 interpolate2(int x, int y, int z, int dx, int dy, int dz);
	vecR3 interpolateEdge(vecN3 pt, int edge);

	vecR3 delta, delta2;

	ValueEvaluator	*thresholdExpr;

	void	Build(number thresh);

	class FieldPrecalculationStage : public CalculationStage
	{
	public:
		IsoSurface *surf;
		
					FieldPrecalculationStage();
	protected:
		virtual void CalculateStage();
	};
	FieldPrecalculationStage precalcStage;
	
	bool	isCyclic;
	number	cycleAt;

	Range	xRange, yRange, zRange;
public:
			IsoSurface();
	virtual	~IsoSurface();
			
	void	SetData(GeneralCubicData *someData);
	void	SetData(ExpressionEvaluator *function);
	void	SetThreshold(ValueEvaluator* thresh);
	void	SetCyclic(number cycle)
			{ isCyclic = true, cycleAt = cycle; }

	void	Draw(GLfloat camera[3], GLfloat light[3]);
	
	void	Build();
	void	SetGLObjectsDirty();

	void	SubmitToBSP();
	void	OutputPOV(ostream& out);
	
	/*RealVariableWatcher	*GetDataFunctionWatcher()
			{	return &dataFunctionWatcher;	}*/
	/*RealVariableWatcher	*GetIsoLevelWatcher()
			{	return &isoLevelWatcher;	}*/

	CalculationStage	*GetFieldEvaluationStage()
			{	return &precalcStage;	}

	void	ReevaluateField();

	virtual bool	GetObjectRanges(Range& xr, Range& yr, Range& zr);

	virtual bool	GetRecommendedBSPResolution(vecN3& res);
};


#endif

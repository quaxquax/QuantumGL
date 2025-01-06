#ifndef _H_ISOSURFACE
#define _H_ISOSURFACE

#include "QuantumMath.h"
#include "VisualObject.h"
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
	bool	isCyclic;
	number	cycleLength;
	
	ValueEvaluator	*thresholdExpr;
	
	class FieldPrecalculationStage : public CalculationStage
	{
	protected:
		virtual void	CalculateStage();
	public:
		IsoSurface	*surf;
		FieldPrecalculationStage();
	};
	
	FieldPrecalculationStage	precalcStage;
	
	Range	xRange, yRange, zRange;
	number  cycleAt;
	
	void Build(number thresh);  // Internal build function
	
public:
	IsoSurface();
	~IsoSurface();
	
	void	SetData(GeneralCubicData *someData);
	void	SetData(ExpressionEvaluator *function);
	void	ReevaluateField();
	
	void	SetThreshold(ValueEvaluator* thresh);
	void	SetCyclic(number cycle)
	{
		isCyclic = true;
		cycleLength = cycle;
		cycleAt = cycle;
	}
	
	void	Draw(GLfloat camera[3], GLfloat light[3]) override;
	void	Build() override;  // Public build function
	void	SetGLObjectsDirty() override;
	void	BuildGLObjects1() override;
	
	void	SubmitToBSP() override;
	void	OutputPOV(ostream& out) override;
	
	bool	GetObjectRanges(Range& xr, Range& yr, Range& zr) override;
	bool	GetRecommendedBSPResolution(vecN3& res) override;

	// Mesh data access methods
	bool HasMeshData() const override { return true; }
	const DynArray<vecR3>* GetVertices() const override { return &theVertices; }
	const DynArray<vecR3>* GetNormals() const override { return &theNormals; }
	const DynArray<GLint>* GetIndices() const override { return &theIndices; }

	// Field evaluation stage access
	CalculationStage* GetFieldEvaluationStage() { return &precalcStage; }
};

#endif

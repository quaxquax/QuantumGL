#include "QuantumConfig.h"
#include GL_GL_H
#include "QuantumMath.h"
#include "CalculationStage.h"
#include "CubicData.h"
#include "DynArray.h"

#include <vector>
using std::vector;
#include <list>
using std::list;

class VisualObject;

class BSPTree : public CalculationStage
{
public:
	struct Vertex		/* 12 * GLfloat */
	{
		GLfloat		texcoord[2];
		GLfloat		color[4];
		GLfloat		normal[3];
		GLfloat		coords[3];
	};
	struct Material
	{
		GLuint		texture;
		GLfloat		alpha;
		GLfloat		shininess;
	};
		
	class Node
	{
	private:
		friend class BSPTree;
		
		GLfloat		a,b,c,d;
		Node*		plusTree;
		Node*		minusTree;
		
		int			material;
		int			nCorners;
		int			vertices;
		
		inline GLfloat dist(GLfloat p[3])
					{ return a*p[0] + b*p[1] + c*p[2] + d; }
		inline bool	IsPlus(GLfloat p[3])
					{ return dist(p) > 0; }
		
		/*void		DrawOne(BSPTree *tree);
		void		Draw(BSPTree *tree, GLfloat camera[3]);*/
		
					Node();
					Node(GLfloat a,GLfloat b,GLfloat c,GLfloat d);
					Node(BSPTree *tree, int mat, int n, int v);
					
		//void		InsertPolygon(BSPTree *tree, int tex, int n, Vertex* p);
	
		void		Statistics(int lev, int& depth, int& mindepth, int& count, int& pcount);
		
		//void		Delete(BSPTree *tree);
		bool		Prune(BSPTree *tree);
	
		Node		*Clone(BSPTree *tree);
	};
private:
	friend class Node;
	
	//Node			*head;
	list<Node>		nodes;
	vector<Vertex>	vertices;
	vector<Material>	materials;
	
	Vertex		*tempVertexStackBase, *tempVertexStackPointer;
	int			tempVertexStackMax,tempVertexStackSize,tempVertexPeakStackSize;

	vecN3				prePartRes;
	CubicData<Node*>	prePartNodes;
	
	/*de*		SplitSpace(Node ** where, GLfloat a,GLfloat b,GLfloat c,GLfloat d);
	Node*		SplitSpace1(Node ** where, vecN3 basicRes, int x);
	Node*		SplitSpace2(Node ** where, vecN3 basicRes, int y);
	Node*		SplitSpace3(Node ** where, vecN3 basicRes, int z);
	void		PrePartition1(Node ** where, vecN3 basicRes, vecN3 maxSize, vecN3 c1, vecN3 c2);
	void		PrePartition2(Node ** where, vecN3 basicRes, vecN3 maxSize, vecN3 c1, vecN3 c2);
	void		PrePartition3(Node ** where, vecN3 basicRes, vecN3 maxSize, vecN3 c1, vecN3 c2);
	void		PrePartition(vecN3 basicRes,vecN3 maxSize);*/

	void		RebuildTree();

	DynArray<Node*> stack;	// stacks for Draw
	DynArray<bool> auxStack;

	GLenum	curBegun;	// gl state
	GLuint	curTex;
	GLfloat	curShine;

	int	nPolygons, nTriangles, nBeginTriangles; // statistics

	void		Draw1(Node* rootNode, GLfloat camera[3]);
	void		Draw2(int x, int y, int z, GLfloat camera[3]);
	void		DrawYSlice(int x, int y, int criticalZ, GLfloat camera[3]);
	void		DrawXSlice(int x, int criticalY, int criticalZ, GLfloat camera[3]);
protected:
	void		CalculateStage();
public:
				BSPTree();
					
	void		Draw(GLfloat camera[3], GLfloat light[3]);
	
	void		SubmitPolygon(int n, Vertex *p, int mat = 0, Node** startNode = NULL);
	Node**		GetSmallCube(vecN3 res, vecN3 pos);

	int			CreateMaterial();
	Material&	GetMaterial(int m);
};

class MaterialUpdater : public CalculationStage
{
public:
				MaterialUpdater();
protected:
	void		CalculateStage();
};

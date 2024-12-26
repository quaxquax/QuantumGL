#include "BSPTree.h"
#include "QuantumDescription.h"
#include "VisualObject.h"
#include "DynArray.h"

#include <functional>
#include <algorithm>
#include <iostream>
#include <cassert>
#include <ctime>

using namespace std;

extern Range worldXRange, worldYRange, worldZRange;

BSPTree::Node::Node()
{
}

BSPTree::Node::Node(GLfloat a, GLfloat b, GLfloat c, GLfloat d)
	: a(a),b(b),c(c),d(d),
		plusTree(NULL), minusTree(NULL),
		material(-1), nCorners(0), vertices(0)
{
}

BSPTree::Node::Node(BSPTree *tree, int mat, int n, int v)
	: plusTree(NULL), minusTree(NULL),
	material(mat), nCorners(n), vertices(v)
{
	assert(n >= 3);
	
	GLfloat *p0,*p1,*p2;
	p0 = tree->vertices[vertices].coords;
	p1 = tree->vertices[vertices+1].coords;
	p2 = tree->vertices[vertices+2].coords;

	GLfloat A[3],B[3];

	A[0] = p1[0] - p0[0];
	A[1] = p1[1] - p0[1];
	A[2] = p1[2] - p0[2];
	B[0] = p2[0] - p0[0];
	B[1] = p2[1] - p0[1];
	B[2] = p2[2] - p0[2];
	a = A[1]*B[2]-A[2]*B[1];
	b = A[2]*B[0]-A[0]*B[2];
	c = A[0]*B[1]-A[1]*B[0];
	
	GLfloat normalizer = 1/sqrt(a*a+b*b+c*c);
	a *= normalizer, b *= normalizer, c *= normalizer;
	
	d = - a * p0[0] - b * p0[1] - c * p0[2];
}

/*void BSPTree::Node::DrawOne(BSPTree *tree)
{
	if(nCorners)
	{
		if(texture)
		{
			glBindTexture(GL_TEXTURE_2D,texture);
			glEnable(GL_TEXTURE_2D);
		}
		glBegin(GL_POLYGON);
		
		vector<Vertex>::iterator p = tree->vertices.begin() + vertices;
		for(int i=0;i<nCorners;i++)
		{
			Vertex& v = *p;
			glNormal3fv(v.normal);
			glColor4fv(v.color);
			if(texture)
				glTexCoord2fv(v.texcoord);
			glVertex3fv(v.coords);
			++p;
		}
		
		glEnd();
		
		if(texture)
			glDisable(GL_TEXTURE_2D);
	}
}

void BSPTree::Node::Draw(BSPTree *tree, GLfloat camera[3])
{
	bool isPlus;
	Node *node1 = NULL, *node2 = NULL;
	
	if(plusTree != NULL || minusTree != NULL)
	{
		isPlus = IsPlus(camera);
		
		if(isPlus)
		{
			node1 = minusTree;
			node2 = plusTree;
		}
		else
		{
			node1 = plusTree;
			node2 = minusTree;
		}
	}
	if(node1)
		node1->Draw(tree,camera);
	
	DrawOne(tree);
	
	if(node2)
		node2->Draw(tree,camera);
}*/

/*
void BSPTree::Node::InsertPolygon(BSPTree *tree, int tex, int n, Vertex* p)
{
	Vertex *plusV = tree->tempVertexStackPointer;
	tree->tempVertexStackPointer += n+2;
	Vertex *minusV = tree->tempVertexStackPointer;
	tree->tempVertexStackPointer += n+2;
	tree->tempVertexStackSize += 2*n+4;
	if(tree->tempVertexStackSize > tree->tempVertexPeakStackSize)
		tree->tempVertexPeakStackSize	= tree->tempVertexStackSize;
		
	assert(tree->tempVertexStackSize <= tree->tempVertexStackMax);
	
	int plusN = 0, minusN = 0;
	
	int wasPlus;
	Vertex oldV;
	
	int i;
	
	const GLfloat EPS = 1e-6;
	GLfloat d1;
	bool wasOncePositive = false, wasOnceNegative = false;
	for(i=0;i<=n;i++)
	{
		Vertex v = p[i%n];
		GLfloat d2 = a*v.coords[0] + b*v.coords[1] + c*v.coords[2] + d;
				//dist(v.coords);

		int isPlus = d2 < -EPS ? -1 : (d2 > EPS ? 1 : 0);
		//	IsPlus(v.coords);
		//int isPlus = theDist < 0 ? -1 : 1;
		if(isPlus > 0)
			wasOncePositive = true;
		if(isPlus < 0)
			wasOnceNegative = true;
		
		if(i != 0)
		{
			if(wasPlus * isPlus < 0)
			{
				Vertex v2;

				if(d1 < 0) d1 = -d1;
				if(d2 < 0) d2 = -d2;
				number pos = d1/(d1+d2);
				number pos1 = 1-pos;
				
				GLfloat *vpOld, *vp, *vp2;
				vpOld = (GLfloat*)&oldV;
				vp = (GLfloat*)&v;
				vp2 = (GLfloat*)&v2;
				
				for(int j=0;j<12;j++)
					vp2[j] = vpOld[j] * pos1 + vp[j] * pos;
				
				assert(plusN < n+2 && minusN < n+2);
				plusV[plusN++] = v2;
				minusV[minusN++] = v2;
			}
		}
		
		if(i != n)
		{
			if(isPlus >= 0)
			{
				assert(plusN < n+2);
				plusV[plusN++] = v;
			}
			if(isPlus <= 0)
			{
				assert(minusN < n+2);
				minusV[minusN++] = v;
			}
		}
		
		wasPlus = isPlus;
		oldV = v;
		d1 = d2;
	}

	if(plusN >= 3 && (wasOncePositive || !wasOnceNegative))
	{
		if(!nCorners && plusN != n)
			cout << "cut on prepart\n";
		if(!plusTree)
		{
			int idx0 = tree->vertices.size();
			tree->vertices.insert(tree->vertices.end(),plusV,plusV+plusN);
			tree->nodes.push_back(Node(tree,tex,plusN,idx0));
			plusTree = &(*--tree->nodes.end());
		}
		else
			plusTree->InsertPolygon(tree, tex, plusN, plusV);
	}
	if(minusN >= 3 && wasOnceNegative)
	{
		if(!nCorners && minusN != n)
			cout << "cut on prepart\n";
		if(!minusTree)
		{
			int idx0 = tree->vertices.size();
			tree->vertices.insert(tree->vertices.end(),minusV,minusV+minusN);
			tree->nodes.push_back(Node(tree,tex,minusN,idx0));
			minusTree = &(*--tree->nodes.end());
		}
		else
			minusTree->InsertPolygon(tree, tex, minusN, minusV);
	}
	if((plusN < 3 && minusN != n) || (minusN < 3 && plusN != n))
	{
		cout << "polygon lost!\n";
	}
	
	tree->tempVertexStackSize -= 2*n+4;
	tree->tempVertexStackPointer -= 2*n+4;
}*/

void BSPTree::Node::Statistics(int lev, int& depth, int& mindepth, int& count, int& pcount)
{
	if(lev > depth)
		depth = lev;
	if(!plusTree && !minusTree)
		if(lev < mindepth)
			mindepth = lev;
	count++;
	if(nCorners)
		pcount++;
	if(plusTree)
		plusTree->Statistics(lev+1,depth,mindepth,count,pcount);
	if(minusTree)
		minusTree->Statistics(lev+1,depth,mindepth,count,pcount);
}

bool BSPTree::Node::Prune(BSPTree *tree)
{
	bool plus = true;
	if(plusTree)
	{
		plus = plusTree->Prune(tree);
		if(plus)
		{
//			plusTree->Delete(tree);
			plusTree = NULL;
		}
	}
	bool minus = true;
	if(minusTree)
	{
		minus = minusTree->Prune(tree);
		if(minus)
		{
//			minusTree->Delete(tree);
			minusTree = NULL;
		}
	}	
	if(nCorners)
		return false;
	else
		return plus && minus;
}

BSPTree::Node *BSPTree::Node::Clone(BSPTree *tree)
{
	Node n = *this;
	if(plusTree)
		n.plusTree = plusTree->Clone(tree);
	if(minusTree)
		n.minusTree = minusTree->Clone(tree);
	tree->nodes.push_back(n);
	return &(*--tree->nodes.end());
}

void BSPTree::Draw1(Node* rootNode, GLfloat camera[3])
{
	stack.append(rootNode);
	auxStack.append(false);

	while(stack.n)
	{
		Node *node = stack.x[--stack.n];
		bool realDraw = auxStack.x[--auxStack.n];
		
		if(realDraw)
		{
			//node->DrawOne(this);
			if(node->nCorners)
			{
				Material& mat = materials[node->material];
				GLuint wantTex = mat.texture;
				//GLenum wantBegin = node->nCorners > 3 ? GL_POLYGON : GL_TRIANGLES;
				if((curBegun && node->nCorners > 3) || curBegun == GL_POLYGON || wantTex != curTex)
					glEnd(), curBegun = 0;

				if(curTex != 0 && wantTex == 0)
				{
					glDisable(GL_TEXTURE_2D), curTex = 0;
					glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				}
				else if(curTex == 0 && wantTex != 0)
				{
					glEnable(GL_TEXTURE_2D);
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				}
				if(wantTex)
					glBindTexture(GL_TEXTURE_2D,(curTex = wantTex));

				if(mat.shininess != curShine)
				{
					if(curShine < 0)
					{
						GLfloat mat_spec1[] = {1,1,1,1};
						glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,mat_spec1);
					}
					curShine = mat.shininess;
					if(curShine < 0)
					{
						GLfloat mat_spec0[] = {0,0,0,1};
						glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,mat_spec0);
					}
					else
						glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,&curShine);
				}

				if(node->nCorners > 3)
					glBegin(GL_POLYGON), curBegun = GL_POLYGON, nPolygons++;
				else if(curBegun != GL_TRIANGLES)
					glBegin(GL_TRIANGLES), curBegun = GL_TRIANGLES, nTriangles++, nBeginTriangles++;
				else
					nTriangles++;


				vector<Vertex>::iterator p = vertices.begin() + node->vertices;
				for(int i=0;i<node->nCorners;i++)
				{
					Vertex& v = *p;
					if(mat.alpha >= 0)
						v.color[3] = mat.alpha;
					glNormal3fv(v.normal);
					glColor4fv(v.color);
					if(curTex)
						glTexCoord2fv(v.texcoord);
					glVertex3fv(v.coords);
					++p;
				}
			}
		}
		else
		{
			Node *node1 = node->plusTree, *node2 = node->minusTree, *node3;
			if(node1 || node2)
			{
				if(!node->IsPlus(camera))
				{
					node3 = node1;
					node1 = node2;
					node2 = node3;
				}
			}
			if(node1)
			{
				stack.append(node1);
				auxStack.append(false);
			}
			stack.append(node);
			auxStack.append(true);
			if(node2)
			{
				stack.append(node2);
				auxStack.append(false);
			}
		}
	}
}

void BSPTree::DrawXSlice(int x, int criticalY, int criticalZ, GLfloat camera[3])
{
	int cy = prePartNodes.cy;
	int y;
	for(y=0;y < cy && y < criticalY;y++)
		DrawYSlice(x,y,criticalZ,camera);
	for(y=cy-1;y >= 0 && y > criticalY;y--)
		DrawYSlice(x,y,criticalZ,camera);
	if(criticalY >= 0 && criticalY < cy)
		DrawYSlice(x,criticalY,criticalZ,camera);
}

void BSPTree::DrawYSlice(int x, int y, int criticalZ, GLfloat camera[3])
{
	int cz = prePartNodes.cz;
	int z;
	for(z=0;z < cz && z < criticalZ;z++)
		Draw2(x,y,z,camera);
	for(z=cz-1;z >= 0 && z > criticalZ;z--)
		Draw2(x,y,z,camera);
	if(criticalZ >=  0 && criticalZ < cz)
		Draw2(x,y,criticalZ,camera);
}

void BSPTree::Draw2(int x, int y, int z, GLfloat camera[3])
{
	if(Node *n = prePartNodes.data[x][y][z])
		Draw1(n,camera);
}

void BSPTree::Draw(GLfloat camera[3], GLfloat light[3])
{
	//CalculateIfNecessary();
	glDepthMask(false);
	glLightModelf (GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glEnable(GL_COLOR_MATERIAL);
	glColor3f(1,1,1);
	glEnable(GL_LIGHTING);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	//head->Draw(this,camera);
	
	stack.n = 0;
	auxStack.n = 0;

	curBegun = 0;
	curTex = 0;
	curShine = -1;
	nPolygons = 0, nTriangles = 0, nBeginTriangles = 0;

	/*glInterleavedArrays(GL_T2F_C4F_N3F_V3F,0,&vertices.front());
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);*/
	
	vecN3 critical;


	critical.x = (int) floor((camera[0] - worldXRange.first) / worldXRange.size() * prePartNodes.cx);
	critical.y = (int) floor((camera[1] - worldYRange.first) / worldYRange.size() * prePartNodes.cy);
	critical.z = (int) floor((camera[2] - worldZRange.first) / worldZRange.size() * prePartNodes.cz);

//	cout << "critical: " << critical.x << " " << critical.y << " " << critical.z << " (" << prePartNodes.cx << ")" << endl;

	int cx = prePartNodes.cx;
	int x;//,y,z;
	for(x=0;x < cx && x < critical.x;x++)
		DrawXSlice(x,critical.y,critical.z,camera);
	for(x=cx-1;x >= 0 && x > critical.x;x--)
		DrawXSlice(x,critical.y,critical.z,camera);
	if(critical.x >= 0 && critical.x < cx)
		DrawXSlice(critical.x,critical.y,critical.z,camera);

	//Draw1(head,camera);
	if(curBegun)
		glEnd();
	if(curTex)
	{
		glDisable(GL_TEXTURE_2D);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
	
	glDepthMask(true);
	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);

	//cout << "stat: " << nPolygons << " polygons, " << nTriangles << " triangles, in "
	//	 << nBeginTriangles << " glBegin(GL_TRIANGLES)/glEnd() pairs." << endl;
}

void BSPTree::RebuildTree()
{
	theVisualObjectsList::iterator p;
	
	nodes.clear();
	vertices.clear();
	
	//int start = glutGet(GLUT_ELAPSED_TIME);
	//vecN3 res = {64,64,64};		// get this from somewhere ###!
	//vecN3 res = {3,3,3};		// get this from somewhere ###!

	vecN3 res = {2,2,2};

	p = theVisualObjects.begin();
	while(p != theVisualObjects.end())
	{
		vecN3 res0;
		if((*p)->GetRecommendedBSPResolution(res0))
		{
			if(res0.x > res.x) res.x = res0.x;
			if(res0.y > res.y) res.y = res0.y;
			if(res0.z > res.z) res.z = res0.z;
		}
		cout << '*' << flush;
		++p;
	}
	
	clock_t startTime = clock();
	cout << "Prepartitioning BSP Tree at "
		 << res.x << "x" << res.y << "x" << res.z <<"\n";

	prePartRes = res;
	//vecN3 maxs = {1,1,1};
	prePartNodes.Allocate(prePartRes.x-1,prePartRes.y-1,prePartRes.z-1);
	for(int i=0;i<prePartNodes.cx;i++)
		for(int j=0;j<prePartNodes.cy;j++)
			for(int k=0;k<prePartNodes.cz;k++)
				prePartNodes.data[i][j][k] = NULL;
	/*PrePartition(prePartRes,maxs);
	cout << "prepartition done in "
		 << double(clock() - startTime)/CLOCKS_PER_SEC
		 << " seconds.\n";*/
	/*if(nodes.begin() != nodes.end())
	{
		cout << "BSP Tree Statistics:\n";
		int depth,mdepth,cnt,pcnt;
		depth = cnt = pcnt = 0;
		mdepth = 32767;
		head->Statistics(1,depth,mdepth,cnt,pcnt);
		cout << "Depth: [" << mdepth << "," << depth << "]\n";
		cout << "Count: " << cnt << "(" << pcnt << ")" << endl;
	}*/
	//int preTime = glutGet(GLUT_ELAPSED_TIME) - start;
	cout << "Rebuilding BSP Tree...\n";
	startTime = clock();
	
	tempVertexPeakStackSize = tempVertexStackSize = 0;
	tempVertexStackMax = 100000;
	tempVertexStackBase = tempVertexStackPointer = new Vertex[tempVertexStackMax];
	
	p = theVisualObjects.begin();
	while(p != theVisualObjects.end())
	{
		(*p)->SubmitToBSP();
		cout << '*' << flush;
		++p;
	}
	cout << "bsp tree done in "
		 << double(clock() - startTime)/CLOCKS_PER_SEC
		 << " seconds.\n";
	delete[] tempVertexStackBase;
	cout << "Peak tempVertex Stack: " << tempVertexPeakStackSize << " vertices.\n";
#if 0
	if(nodes.begin() != nodes.end())
	{
		cout << "BSP Tree Statistics:\n";
		int depth,mdepth,cnt,pcnt;
		depth = cnt = pcnt = 0;
		mdepth = 32767;
		head->Statistics(1,depth,mdepth,cnt,pcnt);
		cout << "Depth: [" << mdepth << "," << depth << "]\n";
		cout << "Count: " << cnt << "(" << pcnt << ")" << endl;
		/*head->Prune(this);
		depth = cnt = pcnt = 0;
		mdepth = 32767;
		head->Statistics(1,depth,mdepth,cnt,pcnt);
		cout << "Depth: [" << mdepth << "," << depth << "]\n";
		cout << "Count: " << cnt << "(" << pcnt << ")" << endl;*/
	}
#endif
	/*int totalTime = glutGet(GLUT_ELAPSED_TIME) - start;
	cout << totalTime/1000.0 << " - " << preTime/1000.0
			 << " = " << totalTime/1000.0-preTime/1000.0 << endl;*/
	//###prePartNodes.Deallocate();
}

/*void BSPTree::SubmitPolygon(int n, Vertex *p, GLuint texture)
{
	if(nodes.size() == 0)
	{
		int idx0 = vertices.size();
		vertices.insert(vertices.end(),p,p+n);
		nodes.push_back(Node(this,texture,n,idx0));
		head = &(*--nodes.end());
	}
	else
	{
		nodes.begin()->InsertPolygon(this, texture, n, p);
	}
}*/

struct BSPTreeSubmitPolygonParams
{
	int n;
	BSPTree::Vertex *p;
	BSPTree::Vertex *sp;
	BSPTree::Node *node;
	vecN3	c1, c2;
};


void BSPTree::SubmitPolygon(int nVertices, Vertex *pVertices, int mat,
	Node** startNode)
//void BSPTree::Node::InsertPolygon(BSPTree *tree, int tex, int n, Vertex* p)
{
	/*
	if(!head)
	{
		int idx0 = vertices.size();
		vertices.insert(vertices.end(),pVertices,pVertices+nVertices);
		nodes.push_back(Node(this,texture,nVertices,idx0));
		head = &(*--nodes.end());
	
		return;
	}*/
	/*Node** xStart = startNode;	//####
	startNode = NULL;
	cout << " ---- *************** ---- \n";*/
	
	/*if(!startNode)
		startNode = &head;*/
	if(!startNode && prePartRes.x <= 2 && prePartRes.y <= 2 && prePartRes.z <= 2)
		startNode = &prePartNodes.data[0][0][0];

	if(startNode && !*startNode)
	{
		int idx0 = vertices.size();
		vertices.insert(vertices.end(),pVertices,pVertices+nVertices);
		nodes.push_back(Node(this,mat,nVertices,idx0));
		*startNode = &(*--nodes.end());

		return;
	}
	
	DynArray<BSPTreeSubmitPolygonParams>	stack;	// I'm sure this is faster than STL... it may waste some memory,
								// but that shouldn't matter here...
	BSPTreeSubmitPolygonParams param;
	param.n = nVertices;
	param.p = pVertices;
	param.sp = tempVertexStackPointer;
	if(startNode)
	{
		param.node = *startNode;
	}
	else
	{
		param.node = NULL;
		param.c1.x = param.c1.y = param.c1.z = 0;
		param.c2.x = prePartRes.x-1;
		param.c2.y = prePartRes.y-1;
		param.c2.z = prePartRes.z-1;
	}
	stack.append(param);

	while(stack.n > 0)
	{
		int n = stack.x[stack.n-1].n;
		Vertex *p = stack.x[stack.n-1].p;
		Vertex *sp = stack.x[stack.n-1].sp;
		Node *node = stack.x[stack.n-1].node;
		
		
		GLfloat a,b,c,d;
		Node **plusTree, **minusTree;
		vecN3 c1_1, c1_2;
		vecN3 c2_1, c2_2;
		if(node)
		{
			a = node->a, b = node->b, c = node->c, d = node->d;
			plusTree = &node->plusTree;
			minusTree = &node->minusTree;
		}
		else
		{
			vecN3 c1,c2;
			c1_1 = c2_1 = c1 = stack.x[stack.n-1].c1;
			c1_2 = c2_2 = c2 = stack.x[stack.n-1].c2;

			if(c2.x - c1.x > 1)
			{
				int x = (c1.x + c2.x) / 2;
				GLfloat xx = -1 + (2 * GLfloat(x)/(prePartRes.x - 1));
				a = 1; b = 0; c = 0; d = -xx;
				c1_2.x = c2_1.x = x;
			}
			else if(c2.y - c1.y > 1)
			{
				int y = (c1.y + c2.y) / 2;
				GLfloat yy = -1 + (2 * GLfloat(y)/(prePartRes.y - 1));
				a = 0; b = 1; c = 0; d = -yy;
				c1_2.y = c2_1.y = y;
			}
			else
			{
				assert(c2.z - c1.z > 1);

				int z = (c1.z+ c2.z) / 2;
				GLfloat zz = -1 + (2 * GLfloat(z)/(prePartRes.z - 1));
				a = 0; b = 0; c = 1; d = -zz;
				c1_2.z = c2_1.z = z;
			}

			if(c1_2.x - c1_1.x == 1 && c1_2.y - c1_1.y == 1 && c1_2.z - c1_1.z == 1)
				minusTree = &prePartNodes.data[c1_1.x][c1_1.y][c1_1.z];
			else
				minusTree = NULL;
			if(c2_2.x - c2_1.x == 1 && c2_2.y - c2_1.y == 1 && c2_2.z - c2_1.z == 1)
				plusTree = &prePartNodes.data[c2_1.x][c2_1.y][c2_1.z];
			else
				plusTree = NULL;
		}

		--stack.n;

		Vertex *plusV = sp;
		sp += n+2;
		Vertex *minusV = sp;
		sp += n+2;
	
		tempVertexStackSize = sp - tempVertexStackBase;
		if(tempVertexStackSize > tempVertexPeakStackSize)
			tempVertexPeakStackSize	= tempVertexStackSize;
			
		assert(tempVertexStackSize <= tempVertexStackMax);
	
		int plusN = 0, minusN = 0;
		
		int wasPlus;
		Vertex oldV;
		
		int i;
		
		const GLfloat EPS = 1e-6;
		GLfloat d1;
		bool wasOncePositive = false, wasOnceNegative = false;
		for(i=0;i<=n;i++)
		{
			Vertex v = p[i%n];
			GLfloat d2 = a*v.coords[0] + b*v.coords[1] + c*v.coords[2] + d;
					//dist(v.coords);
	
			int isPlus = d2 < -EPS ? -1 : (d2 > EPS ? 1 : 0);
			//	IsPlus(v.coords);
			//int isPlus = theDist < 0 ? -1 : 1;
			if(isPlus > 0)
				wasOncePositive = true;
			if(isPlus < 0)
				wasOnceNegative = true;
			
			if(i != 0)
			{
				if(wasPlus * isPlus < 0)
				{
					Vertex v2;
	
					if(d1 < 0) d1 = -d1;
					if(d2 < 0) d2 = -d2;
					number pos = d1/(d1+d2);
					number pos1 = 1-pos;
					
					GLfloat *vpOld, *vp, *vp2;
					vpOld = (GLfloat*)&oldV;
					vp = (GLfloat*)&v;
					vp2 = (GLfloat*)&v2;
					
					for(int j=0;j<12;j++)
						vp2[j] = vpOld[j] * pos1 + vp[j] * pos;
					
					assert(plusN < n+2 && minusN < n+2);
					plusV[plusN++] = v2;
					minusV[minusN++] = v2;
				}
			}
			
			if(i != n)
			{
				if(isPlus >= 0)
				{
					assert(plusN < n+2);
					plusV[plusN++] = v;
				}
				if(isPlus <= 0)
				{
					assert(minusN < n+2);
					minusV[minusN++] = v;
				}
			}
			
			wasPlus = isPlus;
			oldV = v;
			d1 = d2;
		}
	
		if(plusN >= 3 && (wasOncePositive || !wasOnceNegative))
		{
			if(plusTree && !*plusTree)
			{
				int idx0 = vertices.size();
				vertices.insert(vertices.end(),plusV,plusV+plusN);
				nodes.push_back(Node(this,mat,plusN,idx0));
				*plusTree = &(*--nodes.end());
			}
			else
			{
				//if(xStart) cout << "entering plus\n";
				param.n = plusN;
				param.p = plusV;
				param.sp = sp;
				if(plusTree)
					param.node = *plusTree;
				else
				{
					param.node = NULL;
					param.c1 = c2_1;
					param.c2 = c2_2;
				}
				stack.append(param);
			}
		}
		if(minusN >= 3 && wasOnceNegative)
		{
			if(minusTree && !*minusTree)
			{
				int idx0 = vertices.size();
				vertices.insert(vertices.end(),minusV,minusV+minusN);
				nodes.push_back(Node(this,mat,minusN,idx0));
				*minusTree = &(*--nodes.end());
			}
			else
			{
				//if(xStart) cout << "entering minus\n";
				param.n = minusN;
				param.p = minusV;
				param.sp = sp;
				if(minusTree)
					param.node = *minusTree;
				else
				{
					param.node = NULL;
					param.c1 = c1_1;
					param.c2 = c1_2;
				}
				stack.append(param);
			}
		}
		/*if((plusN < 3 && minusN != n) || (minusN < 3 && plusN != n))
		{
			cout << "polygon lost!\n";
		}*/
	}	
}


#if 0

BSPTree::Node* BSPTree::SplitSpace(Node ** where, GLfloat a,GLfloat b,GLfloat c,GLfloat d)
{
	Node n(a,b,c,d);
	/*n.plusTree = nodes.begin()->Clone(this);
	n.minusTree = nodes.begin()->Clone(this);
	nodes.push_front(n);*/
	nodes.push_front(n);
	*where = &*nodes.begin();
	return *where;
}

BSPTree::Node* BSPTree::SplitSpace1(Node ** where, vecN3 basicRes, int x)
{	//############ scaled to???
	GLfloat xx = -1 + (2 * GLfloat(x)/(basicRes.x - 1));
	return SplitSpace(where,1,0,0,-xx);
}

BSPTree::Node* BSPTree::SplitSpace2(Node ** where, vecN3 basicRes, int y)
{
	GLfloat yy = -1 + (2 * GLfloat(y)/(basicRes.y - 1));
	return SplitSpace(where,0,1,0,-yy);
}

BSPTree::Node* BSPTree::SplitSpace3(Node ** where, vecN3 basicRes, int z)
{
	GLfloat zz = -1 + (2 * GLfloat(z)/(basicRes.z - 1));
	return SplitSpace(where,0,0,1,-zz);
}


void BSPTree::PrePartition1(Node ** where, vecN3 basicRes, vecN3 maxSize, vecN3 c1, vecN3 c2)
{
	if(c2.x - c1.x > maxSize.x)
	{
		int splitX = (c1.x + c2.x) / 2;
		
		vecN3 c1_1 = c1, c1_2 = c2;
		vecN3 c2_1 = c1, c2_2 = c2;
		
		c1_2.x = c2_1.x = splitX;

		assert(splitX > c1.x && splitX < c2.x);
		Node *split = SplitSpace1(where,basicRes,splitX);
		
		PrePartition2(&split->minusTree,basicRes,maxSize,c1_1,c1_2);

		//cout << "[" << c1.x << " - " << c2.x << "]\n";
		//cout << "--- x-split at: " << splitX << " ---\n";
		
		PrePartition2(&split->plusTree,basicRes,maxSize,c2_1,c2_2);
	}
	else
	{
		if(c2.y - c1.y > maxSize.y || c2.z - c1.z > maxSize.z)
			PrePartition2(where,basicRes,maxSize,c1,c2);
		else
		{
			//if(c1.x == 8 && c1.y == 27 && c1.z == 26)
			//	cout << ".";
			//cout << "leaf: " << c1.x << ", " << c1.y << ", " << c1.z << endl;
			//assert(prePartNodes.data[c1.x][c1.y][c1.z] == NULL);
			prePartNodes.data[c1.x][c1.y][c1.z] = where;
		}
	}
/*	else
		cout << "leaf: " << (c2.x - c1.x) << " < " << maxSize.x << endl;*/
}

void BSPTree::PrePartition2(Node ** where, vecN3 basicRes, vecN3 maxSize, vecN3 c1, vecN3 c2)
{
	if(c2.y - c1.y > maxSize.y)
	{
		int splitY = (c1.y + c2.y) / 2;
		
		vecN3 c1_1 = c1, c1_2 = c2;
		vecN3 c2_1 = c1, c2_2 = c2;
		
		c1_2.y = c2_1.y = splitY;

		
		assert(splitY > c1.y && splitY < c2.y);
		Node *split = SplitSpace2(where,basicRes,splitY);
		
		PrePartition3(&split->minusTree,basicRes,maxSize,c1_1,c1_2);

		//cout << "--- y-split at: " << splitY << " ---\n";
		
		PrePartition3(&split->plusTree,basicRes,maxSize,c2_1,c2_2);
	}
	else
		PrePartition3(where,basicRes,maxSize,c1,c2);
}

void BSPTree::PrePartition3(Node ** where, vecN3 basicRes, vecN3 maxSize, vecN3 c1, vecN3 c2)
{
	if(c2.z - c1.z > maxSize.z)
	{
		int splitZ = (c1.z + c2.z) / 2;
		
		vecN3 c1_1 = c1, c1_2 = c2;
		vecN3 c2_1 = c1, c2_2 = c2;
		
		c1_2.z = c2_1.z = splitZ;

		assert(splitZ > c1.z && splitZ < c2.z);
		
		Node *split = SplitSpace3(where,basicRes,splitZ);
		
		PrePartition1(&split->minusTree,basicRes,maxSize,c1_1,c1_2);

		//cout << "--- z-split at: " << splitZ << " ---\n";
		
		PrePartition1(&split->plusTree,basicRes,maxSize,c2_1,c2_2);
	}
	else
		PrePartition1(where,basicRes,maxSize,c1,c2);
}


void BSPTree::PrePartition(vecN3 basicRes,vecN3 maxSize)
{
	assert((basicRes.x == basicRes.y) && (basicRes.x == basicRes.z));
	vecN3 nullVec = {0,0,0}, maxVec = {basicRes.x-1, basicRes.y-1, basicRes.z-1};
	/*for(int i=0;i<prePartNodes.cx;i++)
		for(int j=0;j<prePartNodes.cy;j++)
			for(int k=0;k<prePartNodes.cz;k++)
				prePartNodes.data[i][j][k] = NULL;*/

	PrePartition1(&head,basicRes,maxSize,nullVec,maxVec);

	/*for(int i=0;i<prePartNodes.cx;i++)
		for(int j=0;j<prePartNodes.cy;j++)
			for(int k=0;k<prePartNodes.cz;k++)
			{
				assert(prePartNodes.data[i][j][k] != NULL);
				assert(*(prePartNodes.data[i][j][k]) == NULL);
			}*/
}
#endif

void BSPTree::CalculateStage()
{
	RebuildTree();
}

BSPTree::Node**	BSPTree::GetSmallCube(vecN3 res, vecN3 pos)
{
	assert(res.x >= prePartRes.x);
	assert(res.y >= prePartRes.y);
	assert(res.z >= prePartRes.z);
	pos.x /= res.x / prePartRes.x;
	pos.y /= res.y / prePartRes.y;
	pos.z /= res.z / prePartRes.z;

	//return NULL;//&head;// 
	//cout << "returning node for " << pos.x << "," << pos.y << "," << pos.z << endl;
	return &prePartNodes.data[pos.x][pos.y][pos.z];
}

int BSPTree::CreateMaterial()
{
	Material m;
	m.texture = 0;
	m.alpha = -1;
	m.shininess = -1;
	materials.push_back(m);
	return materials.size()-1;
}

BSPTree::Material& BSPTree::GetMaterial(int i)
{
	return materials[i];
}

BSPTree::BSPTree()
{
	SetStageName("BSP Tree");
}

MaterialUpdater::MaterialUpdater()
{
	SetStageName("BSP Tree (Materials)");
}

void MaterialUpdater::CalculateStage()
{
	for_each(theVisualObjects.begin(),theVisualObjects.end(),
			mem_fun(&VisualObject::UpdateMaterials));
}

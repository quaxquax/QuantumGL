#include "IsoSurface.h"
#include "CubeInfo.h"
#include "CubeEdges.h"
#include "ExpressionEvaluator.h"
#include "BSPTree.h"
#include "LockGL.h"

#include <iostream>
#include <fstream>
#include <math.h>

using namespace std;

IsoSurface::FieldPrecalculationStage::FieldPrecalculationStage()
{
	SetStageName("Field for IsoSurface");
}

void IsoSurface::FieldPrecalculationStage::CalculateStage()
{
	surf->ReevaluateField();
}

IsoSurface::IsoSurface()
	: dataFunction(NULL), genData(NULL), thresholdExpr(NULL)
{
	displayListIndex = -1;
	displayListDirty = true;
	precalcStage.surf = this;
	DependOn(&precalcStage);
	isCyclic = false;
	SetStageName("IsoSurface");
}

IsoSurface::~IsoSurface()
{
	if(dataFunction)
		dataFunction->DecRefCount();
	if(genData)
		genData->DecRefCount();
	if(thresholdExpr)
		thresholdExpr->DecRefCount();
	if(displayListIndex != -1)
	{
		LockGL();
		glDeleteLists(displayListIndex,1);
		UnlockGL();
	}
}

void IsoSurface::SetData(GeneralCubicData *someData)
{
	if(genData)
		genData->DecRefCount();
	genData = retain(someData);
	if(genData)
	{
		data = genData->R;
	}
}

void IsoSurface::SetData(ExpressionEvaluator *function)
{
	if(dataFunction != NULL)
		dataFunction->DecRefCount();
	dataFunction = retain(function);
	
	//ReevaluateField();
	precalcStage.MarkForRecalculation();
}

void IsoSurface::ReevaluateField()
{
	GeneralCubicData *data = dataFunction->CreateField();
	SetData(data);
	data->DecRefCount();
}


vecR3 IsoSurface::interpolate2(int x, int y, int z, int dx, int dy, int dz)
{
	number t;
	
	if(!isCyclic)
	{
		number a = data->data[x][y][z] - threshold;
		number b = data->data[x+dx][y+dy][z+dz] - threshold;
		if(a > 0 || b < 0)
			a = -a , b = -b;
			
		t = (-a/(b-a));
		
		if(a > 0 || b < 0)
			cout << "error!!!\n";
	}
	else
	{
		number a = data->data[x][y][z] - threshold;
		number ab = data->data[x+dx][y+dy][z+dz] - data->data[x][y][z];
	
		a = fmod(a,cycleAt);
		ab = fmod(ab,cycleAt);
		if(a < -pi) a += 2*pi;
		if(ab < -pi) ab += 2*pi;
		if(a > pi) a -= 2*pi;
		if(ab > pi) ab -= 2*pi;
		
		t = abs(a/ab);
		if(t > 1)
			cout << "error!!!\n";
	}
	//else		// ###
	//	t = 0.5;*/


	vecR3 p;// = coord(x,y,z);
	p.x = xRange.first + x*delta.x + dx*t*delta.x;
	p.y = yRange.first + y*delta.y + dy*t*delta.y;
	p.z = zRange.first + z*delta.z + dz*t*delta.z;
	
	return p;
}

vecR3 IsoSurface::interpolateEdge(vecN3 pt, int edge)
{
	switch(edge)
	{
		case	0:	return interpolate2(pt.x, pt.y, pt.z, 1,0,0);
		case	1:	return interpolate2(pt.x+1, pt.y, pt.z, 0,1,0);
		case	2:	return interpolate2(pt.x, pt.y+1, pt.z, 1,0,0);
		case	3:	return interpolate2(pt.x, pt.y, pt.z, 0,1,0);
		case	4:	return interpolate2(pt.x, pt.y, pt.z+1, 1,0,0);
		case	5:	return interpolate2(pt.x+1, pt.y, pt.z+1, 0,1,0);
		case	6:	return interpolate2(pt.x, pt.y+1, pt.z+1, 1,0,0);
		case	7:	return interpolate2(pt.x, pt.y, pt.z+1, 0,1,0);
		case	8:	return interpolate2(pt.x, pt.y, pt.z, 0,0,1);
		case	9:	return interpolate2(pt.x+1, pt.y, pt.z, 0,0,1);
		case	10:	return interpolate2(pt.x+1, pt.y+1, pt.z, 0,0,1);
		case	11:	return interpolate2(pt.x, pt.y+1, pt.z, 0,0,1);
	}
	return vecR3();	// keep compiler silent
}

inline int number2int(number x, int cx)
{
	return (int)floor((x+1)/2 * (cx-1) + 0.5);
}

static bool cyclicCompare(number val, number cycle, GLubyte& type, int bitmask)
{
	val = fmod(val,cycle);
	while(val < -cycle/2)
		val += cycle;
	while(val >= cycle/2)
		val -= cycle;
	if(val > cycle/4 || val < -cycle/4)
		return false;
		
	if(val > 0)
		type |= bitmask;
		
	return true;
}

void IsoSurface::Build(number thresh)
{
	int	isoStatistics[256];
	bool	uncoveredCases = false;
	
	threshold = thresh;
	
	DynArray<Critical> criticals;

	cout << "Calculating Surface at " << thresh << "\n";

	dataFunction->GetRanges(xRange,yRange,zRange);
	
	for(int i=0;i<256;i++)
		isoStatistics[i] = 0;
	
	int	iCutMin, iCutMax, jCutMin, jCutMax, kCutMin, kCutMax;
	iCutMin = number2int(xCut.first,data->cx);
	iCutMax = number2int(xCut.second,data->cx);
	jCutMin = number2int(yCut.first,data->cy);
	jCutMax = number2int(yCut.second,data->cy);
	kCutMin = number2int(zCut.first,data->cz);
	kCutMax = number2int(zCut.second,data->cz);
	
	for(int i=0;i<data->cx-1;i++)
	{
		for(int j=0;j<data->cy-1;j++)
		{
			for(int k=0;k<data->cz-1;k++)
			{
				if(i >= iCutMin && i <= iCutMax
				 &&j >= jCutMin && j <= jCutMax
				 &&k >= kCutMin && k <= kCutMax)
					continue;
				/*if(i != 0 || j != 0 || k != 0)
					break;*/

				GLubyte type = 0;
				if(!isCyclic)
				{
					if(data->data[i][j][k] > thresh)
						type |= (1 << 0);
					if(data->data[i+1][j][k] > thresh)
						type |= (1 << 1);
					if(data->data[i+1][j+1][k] > thresh)
						type |= (1 << 2);
					if(data->data[i][j+1][k] > thresh)
						type |= (1 << 3);
					if(data->data[i][j][k+1] > thresh)
						type |= (1 << 4);
					if(data->data[i+1][j][k+1] > thresh)
						type |= (1 << 5);
					if(data->data[i+1][j+1][k+1] > thresh)
						type |= (1 << 6);
					if(data->data[i][j+1][k+1] > thresh)
						type |= (1 << 7);
				}
				else
				{
					if(!cyclicCompare(data->data[i][j][k]-thresh,cycleAt,type,1<<0))
						continue;
					if(!cyclicCompare(data->data[i+1][j][k]-thresh,cycleAt,type,1<<1))
						continue;
					if(!cyclicCompare(data->data[i+1][j+1][k]-thresh,cycleAt,type,1<<2))
						continue;
					if(!cyclicCompare(data->data[i][j+1][k]-thresh,cycleAt,type,1<<3))
						continue;
					if(!cyclicCompare(data->data[i][j][k+1]-thresh,cycleAt,type,1<<4))
						continue;
					if(!cyclicCompare(data->data[i+1][j][k+1]-thresh,cycleAt,type,1<<5))
						continue;
					if(!cyclicCompare(data->data[i+1][j+1][k+1]-thresh,cycleAt,type,1<<6))
						continue;
					if(!cyclicCompare(data->data[i][j+1][k+1]-thresh,cycleAt,type,1<<7))
						continue;
					/*if(k != 60)
						type = 0;*/
				}
				//cout << hex << (int)type << dec << endl;
				if(type != 0 && type != 255)
				{
					Critical cube;
					cube.cube.x = i;
					cube.cube.y = j;
					cube.cube.z = k;
					cube.type = type;
					criticals.append(cube);
				}
				
				if(!cubeInfo[type].inited)
					isoStatistics[type]++, uncoveredCases = true;
			}
		}
	}
	cout << " => " << criticals.n << " critical cubes.\n";

	if(uncoveredCases)
	{
		cout << "Not all special cases covered!!!\n";
		ofstream out("isostat.txt");
		for(int i=0;i<256;i++)
			if(isoStatistics[i])
				out << isoStatistics[i] << "\t" << i << "\t" << hex << i << dec << endl;
	}
	
	/*delta = coord(1,1,1); 
	delta2 = coord(0,0,0);
	delta.x -= delta2.x, delta2.x = delta.x * 0.5;
	delta.y -= delta2.y, delta2.y = delta.y * 0.5;
	delta.z -= delta2.z, delta2.z = delta.z * 0.5;*/
	delta.x = (xRange.second - xRange.first)/(data->cx-1);
	delta.y = (yRange.second - yRange.first)/(data->cy-1);
	delta.z = (zRange.second - zRange.first)/(data->cz-1);
	delta2.x = delta.x * 0.5;
	delta2.y = delta.y * 0.5;
	delta2.z = delta.z * 0.5;
	
	theVertices.n = theNormals.n = theColors.n = 
			theIndices.n = theCubes.n = 0;
	
	CubeEdges indexBuffer(data->cx,data->cy,data->cz);
	DynArray<GLint> normalCount;
	
	for(int i=0;i<criticals.n;i++)
	{		
		if(cubeInfo[criticals.x[i].type].inited)
		{
			CubeInfo inf = cubeInfo[criticals.x[i].type];
			vecN3 cube = criticals.x[i].cube;
		
			for(int j=0;j<inf.nTriangles;j++)
			{
				int k;
				
				vecR3 points[3];
				vecR3 normal;
				vecR3 zero3;
				zero3.x = zero3.y = zero3.z = 0;
				
				GLint indices[3];
				for(k=0;k<3;k++)
				{
					int edge = inf.edges[j][k];
					indices[k] = indexBuffer.ReadEdge(cube.x, cube.y, cube.z,
														edge);
					if(indices[k] == -1)
					{
						indices[k] = theVertices.n;
						indexBuffer.WriteEdge(cube.x, cube.y, cube.z, edge,
												indices[k]);
						
						points[k] = interpolateEdge(cube,edge);
						theVertices.append(points[k]);
						theNormals.append(zero3);
						normalCount.append(0);
						
						/*vecR3 color = points[k];
						color.x = color.x/2 + 0.5;
						color.y = color.y/2 + 0.5;
						color.z = color.z/2 + 0.5;
						theColors.append(color);		//###*/
						if(colorFunction)
						{
//							colorFunction->GetColor(points[k])
							vecR3 color;
							colorFunction->EvaluateReal(
								points[k],3,(number*)(&color));
							theColors.append(color);
						}
						else
						{
							vecR3 color = { 1, 1, 1 };
							theColors.append(color);
						}
					}
					else
					{
						points[k] = theVertices.x[indices[k]];
					}	
					theIndices.append(indices[k]);
				}
				theCubes.append(criticals.x[i].cube);
			
				vecR3 a,b;
				a.x = points[1].x - points[0].x;
				a.y = points[1].y - points[0].y;
				a.z = points[1].z - points[0].z;
				b.x = points[2].x - points[0].x;
				b.y = points[2].y - points[0].y;
				b.z = points[2].z - points[0].z;
				normal.x = a.y*b.z-a.z*b.y;
				normal.y = a.z*b.x-a.x*b.z;
				normal.z = a.x*b.y-a.y*b.x;
				/*number len = normal.x*normal.x
							+normal.y*normal.y
							+normal.z*normal.z;
				len = 1/sqrt(len);
				normal.x *= len, normal.y *= len, normal.z *= len;*/
				
				for(k=0;k<3;k++)
				{
					theNormals.x[indices[k]].x += normal.x;
					theNormals.x[indices[k]].y += normal.y;
					theNormals.x[indices[k]].z += normal.z;
					normalCount.x[indices[k]]++;
				}
			}
		}
	}
	//GLdouble minNorm, maxNorm;
	for(int i=0;i<theNormals.n;i++)
	{
		//GLdouble factor = 1.0/normalCount.x[i];
		if(normalCount.x[i] == 0)
		{
			cout << "normalCount is zero!!!\n";
		}
		if(theNormals.x[i].x == 0 && theNormals.x[i].y == 0 && theNormals.x[i].z == 0)
		{
			cout << "normal vector is zero!\n";
		}
		GLdouble norm2 = theNormals.x[i].x*theNormals.x[i].x
						+theNormals.x[i].y*theNormals.x[i].y
						+theNormals.x[i].z*theNormals.x[i].z;
		GLdouble factor = 1/sqrt(norm2);

		theNormals.x[i].x *= factor;
		theNormals.x[i].y *= factor;
		theNormals.x[i].z *= factor;
		
	/*	if(i == 0)
			minNorm = maxNorm = norm2;
		else
		{
			if(norm2 < minNorm)
				minNorm = norm2;
			if(norm2 > maxNorm)
				maxNorm = norm2;
		}*/
	}
	//minNorm = sqrt(minNorm);
	//maxNorm = sqrt(maxNorm);
	//cout << minNorm << " <= |n| <= " << maxNorm << endl;
	cout << " => " << theIndices.n/3 << " triangles.\n";
	cout << " => " << (theIndices.n - theVertices.n)*100.0/theIndices.n
				 << "% of vertices folded.\n";

	displayListDirty = true;
}

void IsoSurface::Draw(GLfloat camera[3], GLfloat light[3])
{
#if 1
	if(transparencyExpr)
		return;
	if(displayListIndex == -1)
	{
		displayListIndex = glGenLists(1);	
		displayListDirty = true;
	}
	if(displayListDirty)
	{
		displayListDirty = false;
		
		glNewList(displayListIndex,GL_COMPILE_AND_EXECUTE);
		
			GLfloat mat_diffuse[] = { 1.0, 1.0, 0.0, /*0.5*/1 };
			glLightModelf (GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
			glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
			SetupShininess();
				
		//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//	glEnable(GL_BLEND);
			glEnable(GL_COLOR_MATERIAL);
		//	glColor3f(1,0,0);
			glEnable(GL_LIGHTING);
			
			glEnableClientState(GL_NORMAL_ARRAY);
			glEnableClientState(GL_COLOR_ARRAY);
			glEnableClientState(GL_VERTEX_ARRAY);
		
			glNormalPointer(GL_FLOAT,0,theNormals.x);
			glColorPointer(3,GL_FLOAT,0,theColors.x);
			glVertexPointer(3,GL_FLOAT,0,theVertices.x);
		
			glDrawElements(GL_TRIANGLES, theIndices.n, 
						GL_UNSIGNED_INT, theIndices.x);
			
			glDisableClientState(GL_NORMAL_ARRAY);
			glDisableClientState(GL_VERTEX_ARRAY);
				
			glDisable(GL_LIGHTING);

			glDisable(GL_BLEND);
			
			/*glPointSize(10.0);
			glBegin(GL_POINTS);
			for(int i=0;i<data->cx;i++)
			{
				for(int j=0;j<data->cy;j++)
				{
					for(int k=0;k<data->cz;k++)
					{
						bool inside = data->data[i][j][k] > threshold;
						glColor3f(inside ? 0 : 1,inside ? 1 : 0, 0);
						glVertex3f(
							-1 + 2*(i / double(data->cx-1)),
							-1 + 2*(j / double(data->cy-1)),
							-1 + 2*(k / double(data->cz-1)));
					}
				}
			}
			glEnd();*/

		glEndList();
	}
	else
	{
		glCallList(displayListIndex);
	}
#endif
}

void IsoSurface::SubmitToBSP()
{
	VisualObject::SubmitToBSP();
	if(!transparencyExpr)
		return;
	/*GLfloat trans;
	{
		number tmp;
		transparencyExpr->EvaluateReal(1,&tmp);
		trans = Range(0,1).clamp((GLfloat)tmp);
	}*/

	Range xRange, yRange, zRange;
	dataFunction->GetRanges(xRange,yRange,zRange);
	extern Range worldXRange, worldYRange, worldZRange;
	bool fullRange = xRange == worldXRange
					&& yRange == worldYRange
					&& zRange == worldZRange;
	
	vecN3 res;
	res.x = data->cx;
	res.y = data->cy;
	res.z = data->cz;
	
	BSPTree::Vertex vertices[3];
	for(int i=0;i<theIndices.n;i++)
	{
		if(i % 3000 == 0)
			cout << (i/3) << "/" << (theIndices.n/3) << endl;
		int ii = i%3;
		int idx = theIndices.x[i];
		vertices[ii].coords[0] = theVertices.x[idx].x;
		vertices[ii].coords[1] = theVertices.x[idx].y;
		vertices[ii].coords[2] = theVertices.x[idx].z;
		vertices[ii].color[0] = theColors.x[idx].x;
		vertices[ii].color[1] = theColors.x[idx].y;
		vertices[ii].color[2] = theColors.x[idx].z;
		vertices[ii].color[3] = 0;//trans;
		vertices[ii].normal[0] = theNormals.x[idx].x;
		vertices[ii].normal[1] = theNormals.x[idx].y;
		vertices[ii].normal[2] = theNormals.x[idx].z;
		vertices[ii].texcoord[0] = 0;
		vertices[ii].texcoord[1] = 0;

		
		
		if(ii == 2)
		{
			vecN3 pos = theCubes.x[i/3];
			BSPTree::Node **startNode = 
				fullRange ?
						BSP->GetSmallCube(res,pos)
					:   NULL;
			BSP->SubmitPolygon(3,vertices,mainMaterial,startNode);
		}
			
	}
	cout << (theIndices.n/3) << "/" << (theIndices.n/3) << endl;
}

static void outPOVVec(ostream& out,vecR3 vec)
{
	out << '<' << vec.x << ',' << vec.y << ',' << vec.z << '>';
}

void IsoSurface::OutputPOV(ostream& out)
{
	number trans;
	if(transparencyExpr)
		transparencyExpr->EvaluateReal(1,&trans);
	else
		trans = 0;
	trans = Range(0,1).clamp((GLfloat)trans);
	
	for(int i=0;i<theIndices.n;i+=3)
	{
		int idx1 = theIndices.x[i];
		int idx2 = theIndices.x[i+1];
		int idx3 = theIndices.x[i+2];
#if 1
		out << "smooth_triangle {\n\t";
		outPOVVec(out,theVertices.x[idx1]); out << ", ";
		outPOVVec(out,theNormals.x[idx1]); out << ",\n";
		outPOVVec(out,theVertices.x[idx2]); out << ", ";
		outPOVVec(out,theNormals.x[idx2]); out << ",\n";
		outPOVVec(out,theVertices.x[idx3]); out << ", ";
		outPOVVec(out,theNormals.x[idx3]);
#else
		out << "triangle {\n\t";
		outPOVVec(out,theVertices.x[idx1]); out << ",\n";
		outPOVVec(out,theVertices.x[idx2]); out << ",\n";
		outPOVVec(out,theVertices.x[idx3]);
#endif
		out << "\n\n\tpigment { color rgbt <";
		out << theColors.x[idx1].x << ',' << theColors.x[idx1].y << ',' 
			<< theColors.x[idx1].z << ',' << trans << "> }\n}\n";
	}
}


void IsoSurface::SetThreshold(ValueEvaluator* thresh)
{
	if(thresholdExpr)
		thresholdExpr->DecRefCount();
	thresholdExpr = retain(thresh);
}

void IsoSurface::SetGLObjectsDirty()
{
	VisualObject::SetGLObjectsDirty();
	displayListDirty = true;
}

void IsoSurface::Build()
{
	if(thresholdExpr)
	{
		number thresh;
		thresholdExpr->EvaluateReal(1,&thresh);
		Build(thresh);
	}
	else
		Build(0.5);
}

bool IsoSurface::GetObjectRanges(Range& xr, Range& yr, Range& zr)
{
	assert(dataFunction);
	dataFunction->GetRanges(xr,yr,zr);
	return true;
}

bool IsoSurface::GetRecommendedBSPResolution(vecN3& res)
{
	if(!transparencyExpr)
		return false;
	CalculateIfNecessary();
	res.x = data->cx;
	res.y = data->cy;
	res.z = data->cz;

	return true;		// ####
}

#include "CubeInfo.h"

#include <iostream>
using namespace std;

struct BasicCase
{
	unsigned char	theCase;
	int	nTriangles;	// <= kCubeMaxTriangles
	int	edges[kCubeMaxTriangles][3];
};

const int	nBasicCases = 14 + 3;	// should be 15
const BasicCase basicCases[] = {
	{	0x00,	0,	{}	},				// 0
	{	0x01,	1,	{{0,3,8}}	},		// 1
	{	0x05,	4,	{{3,8,2},{8,10,2},{8,0,1},{8,1,10}}	},	// 2	INVERT!
	{	0x09,	2,	{{0,2,8},{8,2,11}}	},					// 2
	{	0x41,	2,	{{0,3,8},{10,5,6}}	},	// 2
	{	0x07,	3,	{{8,9,10},{8,10,2},{8,2,3}}	},			// 3
 	{	0x29,	5,	{{9,0,2},{9,2,5}, {2,11,5}, {11,8,4},{11,4,5}}	},	// 3	INVERT!
	{	0x25,	5,	{{0,1,9}, {2,3,8}, {2,8,10}, {8,4,10}, {10,4,5}}	},	// 3 problem	INVERT!
	{	0x0f,	2,	{{8,9,10},{8,10,11}}	},				// 4
	{	0x1b,	4,	{{1,2,11},{1,11,7},{1,7,4},{1,4,9}}	},	// 4
	{	0x55,	4,	{{3,7,2},{7,6,2},{4,0,1},{1,5,4}}	},	// 4
	{	0x1e,	4,	{{8,4,7},{0,3,11},{0,11,10},{0,10,9}}	},	// 4
	{	0x2e,	4,	{{0,11,3}, {0,10,11}, {0,4,10}, {4,5,10}}	},	// 4
	{	0xa5,	4,	{{10,6,5}, {8,4,7}, {2,3,11}, {0,1,9}}	},	// 4
	
	{	0xFA,	2,	{{8,3,0}, {2,10,1}}	},		// 2	~0x05
	{	0xD6,	3,	{{8,2,0}, {8,11,2}, {4,9,5}}	},	// 3	~0x29
	{	0xDA,	3,	{{8,3,0}, {2,1,10}, {4,9,5}}	}	// 3	~0x25
};

CubeInfo	cubeInfo[256];

static unsigned char BasicTransformCombi(unsigned char c, int bt)
{
	unsigned char transCorner[4][8] =
	{
		{	// rotate x
			4, 5, 1, 0,
			7, 6, 2, 3
		},
		{	// rotate y
			1, 5, 6, 2,
			0, 4, 7, 3
		},
		{	// rotate z
			1, 2, 3, 0,
			5, 6, 7, 4
		},
		{	// mirror
			6, 7, 4, 5,
			2, 3, 0, 1
		}
	};
	
	unsigned char result = 0;
	
	for(int shift = 0;shift < 8;shift++)
	{
		if(c & (1 << shift))
			result |= (1 << transCorner[bt][shift]);
	}
	
	return result;
}

static unsigned char BasicTransormEdge(unsigned char c, int bt)
{
	unsigned char transEdge[4][12] =
	{
		{	// rotate x
			4, 9, 0, 8,
			6, 10, 2, 11,
			7, 5, 1, 3
		},
		{	// rotate y
			9, 5, 10, 1,
			8, 7, 11, 3,
			0, 4, 6, 2
		},
		{	// rotate z
			1, 2, 3, 0,
			5, 6, 7, 4,
			9, 10, 11, 8
		},
		{	// mirror
			6, 7, 4, 5,
			2, 3, 0, 1,
			10, 11, 8, 9
		}
	};
	return transEdge[bt][c];
}

static unsigned char TransformCombi(unsigned char c, int t)
{
	int flip = t%2;
	int rot = (t/2)%4;
	int face = t/8;
	if(flip)
		c = BasicTransformCombi(c,3);	// mirror
	
	if(face == 4)
		c = BasicTransformCombi(c,1);	// rotate y
	else if(face == 5)
		c = BasicTransformCombi(BasicTransformCombi(BasicTransformCombi
					(c,1),1),1);		// (rotate y)^3
	else
	{
		while(face--)
			c = BasicTransformCombi(c,0);	// rotate x
	}
	while(rot--)
		c = BasicTransformCombi(c,2);		// rotate z
		
	return c;
}

static unsigned char TransformEdge(unsigned char c, int t)
{
	int flip = t%2;
	int rot = (t/2)%4;
	int face = t/8;
	if(flip)
		c = BasicTransormEdge(c,3);	// mirror
	
	if(face == 4)
		c = BasicTransormEdge(c,1);	// rotate y
	else if(face == 5)
		c = BasicTransormEdge(BasicTransormEdge(BasicTransormEdge
					(c,1),1),1);	// (rotate y)^3
	else
	{
		while(face--)
			c = BasicTransormEdge(c,0);		// rotate x
	}
	while(rot--)
		c = BasicTransormEdge(c,2);			// rotate z

	return c;
}

static void InitBasicCase(int theCase, bool invert)
{
	for(int i=0;i<48;i++)
	{
		unsigned char combi = TransformCombi(
			invert ? (~basicCases[theCase].theCase)
				: basicCases[theCase].theCase, i);
		
		if(!cubeInfo[combi].inited)
		{
			cubeInfo[combi].inited = true;
			
		/*	cout << "inited 0x" << hex << combi << " from " 
					<< basicCases[theCase].theCase << dec
					<< ":" << i << endl;*/
			
			int n;
			
			bool xinvert = invert ? (i%2) : !(i%2);
			cubeInfo[combi].nTriangles = n = basicCases[theCase].nTriangles;
			for(int j=0;j<n;j++)
			{
				for(int k=0;k<3;k++)
					cubeInfo[combi].edges[j][k] =
						TransformEdge(basicCases[theCase].edges[j][xinvert?2-k:k],i);
			}
		}
	}
}

void InitCubeInfo()
{
	int i;
	for(i=0;i<256;i++)
		cubeInfo[i].inited = false;
	
	for(i=0;i<nBasicCases;i++)
		InitBasicCase(i,false);		
	for(i=0;i<nBasicCases;i++)
		InitBasicCase(i,true);
	for(i=0;i<256;i++)
		if(!cubeInfo[i].inited)
			cout << "uninited case: " << hex << i << dec << endl;
}

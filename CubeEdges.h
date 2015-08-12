#ifndef _H_CUBE_EDGES
#define _H_CUBE_EDGES

#include "CubicData.h"

class CubeEdges
{
	CubicData<int>	xedges, yedges, zedges;
public:
			CubeEdges(int cx, int cy, int cz);
	
	int		ReadEdge(int x, int y, int z, int edge);
	void	WriteEdge(int x, int y, int z, int edge, int value);
};

#endif

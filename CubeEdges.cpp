#include "CubeEdges.h"

CubeEdges::CubeEdges(int cx, int cy, int cz)
{
	xedges.Allocate(cx-1,cy,cz);
	yedges.Allocate(cx,cy-1,cz);
	zedges.Allocate(cx,cy,cz-1);
	for(int x=0;x<cx;x++)
		for(int y=0;y<cy;y++)
			for(int z=0;z<cz;z++)
			{
				if(x != cx-1)
					xedges.data[x][y][z] = -1;
				if(y != cy-1)
					yedges.data[x][y][z] = -1;
				if(z != cz-1)
					zedges.data[x][y][z] = -1;
			}
}

int CubeEdges::ReadEdge(int x, int y, int z, int edge)
{
	switch(edge)
	{
		case 0: return xedges.data[x][y][z];
		case 1:	return yedges.data[x+1][y][z];
		case 2: return xedges.data[x][y+1][z];
		case 3: return yedges.data[x][y][z];
		case 4: return xedges.data[x][y][z+1];
		case 5:	return yedges.data[x+1][y][z+1];
		case 6: return xedges.data[x][y+1][z+1];
		case 7: return yedges.data[x][y][z+1];
		case 8: return zedges.data[x][y][z];
		case 9: return zedges.data[x+1][y][z];
		case 10: return zedges.data[x+1][y+1][z];
		case 11: return zedges.data[x][y+1][z];
	}
	return -1;
}

void CubeEdges::WriteEdge(int x, int y, int z, int edge, int value)
{
	switch(edge)
	{
		case 0: xedges.data[x][y][z] = value;	break;
		case 1:	yedges.data[x+1][y][z] = value;	break;
		case 2: xedges.data[x][y+1][z] = value;	break;
		case 3: yedges.data[x][y][z] = value;	break;
		case 4: xedges.data[x][y][z+1] = value;	break;
		case 5:	yedges.data[x+1][y][z+1] = value;	break;
		case 6: xedges.data[x][y+1][z+1] = value;	break;
		case 7: yedges.data[x][y][z+1] = value;	break;
		case 8: zedges.data[x][y][z] = value;	break;
		case 9: zedges.data[x+1][y][z] = value;	break;
		case 10: zedges.data[x+1][y+1][z] = value;	break;
		case 11: zedges.data[x][y+1][z] = value;	break;
	}
}

const int kCubeMaxTriangles = 5;

struct CubeInfo
{
	bool	inited;
	int nTriangles;
	int edges[kCubeMaxTriangles][3];
};

extern CubeInfo	cubeInfo[256];

void InitCubeInfo();

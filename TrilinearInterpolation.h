#ifndef _H_TRILINEAR_INTERPOLATION
#define _H_TRILINEAR_INTERPOLATION
#include <math.h>

template <class T>
	T LinearInterpolation(T a, T b, number pos)
	{
		return a * (1-pos) + b * pos;
	}

template <class T>
	T TrilinearInterpolation(CubicData<T> *data, vecR3 pos, int comp=0)
	{
#if 0
		int x,y,z;
		x = (int)floor((pos.x+1)/2 * (data->cx-1) + 0.5);
		y = (int)floor((pos.y+1)/2 * (data->cy-1) + 0.5);
		z = (int)floor((pos.z+1)/2 * (data->cz-1) + 0.5);
	
		/*extern vecN3 intPos;
		if(x != intPos.x || y != intPos.y || z != intPos.z)
		{
			cout << "(" << x << "|" << y << "|" << z << ") instead of ("
				<< intPos.x << "|" << intPos.y << "|" << intPos.z << ")\n";
		}*/
		int n = data->n;
		return data->data[x][y][z*n+comp];
#else
		int x,y,z;
		number fx, fy, fz;
		fx = (pos.x+1)/2 * (data->cx-1);
		fy = (pos.y+1)/2 * (data->cy-1);
		fz = (pos.z+1)/2 * (data->cz-1);
		x = (int)fx;
		y = (int)fy;
		z = (int)fz;

		//if(x == data->cx-1) cout << "x";
		
		int n = data->n;

		number ix = fx - x;
		number iy = fy - y;
		number iz = fz - z;

		if(x < 0 || y < 0 || z < 0 || x >= data->cx || y >= data->cy || z >= data->cz)
			return 0;	// coordinates out of range

/*		if(x == data->cx-1
		 ||y == data->cy-1
		 ||z == data->cz-1)
			return data->data[x][y][z*n+comp];*/
		if(x == data->cx-1)
			--x, ix = 1;
		if(y == data->cy-1)
			--y, iy = 1;
		if(z == data->cz-1)
			--z, iz = 1;
		
		return	LinearInterpolation(
					LinearInterpolation(
						LinearInterpolation(
							data->data[x][y][z*n+comp],
							data->data[x+1][y][z*n+comp],
							ix),
						LinearInterpolation(
							data->data[x][y+1][z*n+comp],
							data->data[x+1][y+1][z*n+comp],
							ix),
						iy),
					LinearInterpolation(
						LinearInterpolation(
							data->data[x][y][(z+1)*n+comp],
							data->data[x+1][y][(z+1)*n+comp],
							ix),
						LinearInterpolation(
							data->data[x][y+1][(z+1)*n+comp],
							data->data[x+1][y+1][(z+1)*n+comp],
							ix),
						iy),
				iz);
#endif
	}
	
#endif

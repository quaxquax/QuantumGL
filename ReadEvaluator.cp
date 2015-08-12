#include "ReadEvaluator.h"
#include "CubicData.h"
#include <fstream>
#include <iostream>
#include <string.h>
#include <stdio.h>
using namespace std;

#define STDIO_BIN

const short endianTest = 1;
const bool&	isLittleEndian = (bool) *((char*)&endianTest);

static void swapshort(unsigned short *s)
{
	if(isLittleEndian)
	{
		unsigned char c,*p = (unsigned char*) s;
		c = p[0];
		p[0] = p[1];
		p[1] = c;
	}
}

ReadEvaluator::ReadEvaluator(const char* fn)
{
	strcpy(filename,fn);
	{
		ifstream in(filename);

		char ch;
		in >> ch;
		bin = false;
		if(ch == 'B')
			bin = true;
		else
		{
			in.unget();
			int tmp;
			in >> tmp;
			c = tmp != 0;
			in >> n;
			in >> res.x >> res.y >> res.z;
		}
	}
	if(bin)
	{
#ifdef STDIO_BIN
		FILE *f = fopen(filename,"rb");
		fseek(f,4,SEEK_SET);
		char ch = (char)fgetc(f);
		c = ch == 'C';
		ch = (char)fgetc(f);
		n = (unsigned char) ch;
		
		unsigned short tmpS;
		fread((char*)&tmpS,2,1,f);
		swapshort(&tmpS);
		res.x = tmpS;
		fread((char*)&tmpS,2,1,f);
		swapshort(&tmpS);
		res.y = tmpS;
		fread((char*)&tmpS,2,1,f);
		swapshort(&tmpS);
		res.z = tmpS;

		fclose(f);
#else
		ifstream in(filename,ios::binary);

		char ch;
		in.get(), in.get(), in.get(), in.get();
		ch = in.get();
		c = ch == 'C';
		ch = in.get();
		n = (unsigned char) ch;
		
		unsigned short tmpS;
		in.read((char*)&tmpS,2);
		swapshort(&tmpS);
		res.x = tmpS;
		in.read((char*)&tmpS,2);
		swapshort(&tmpS);
		res.y = tmpS;
		in.read((char*)&tmpS,2);
		swapshort(&tmpS);
		res.z = tmpS;
#endif
	}
	xRange = yRange = zRange = Range(-1,1);
}

GeneralCubicData* ReadEvaluator::DoCreateField(vecN3 res, bool forceRes)
{
	res = this->res;	///****#############
	
	GeneralCubicData *d = new GeneralCubicData();
	
	if(c)
		d->C = new CubicData< complex<number> >(res.x, res.y, res.z, n);
	else
		d->R = new CubicData< number >(res.x, res.y, res.z, n);
	
	cout << "       ";
	for(unsigned i=0;i<strlen(filename);i++)
		cout << " ";
	cout << "[";
	for(int i=0;i<res.x;i++)
		cout << " ";
	cout << "]\n";
	cout << "Loading " << filename;

	number theMin = 0, theMax = 0;
	bool	first = true;

	if(!bin)
	{
		ifstream in(filename);
		
		int tmp;
		in >> tmp >> tmp >> tmp >> tmp >> tmp;
		
		for(int i=0;i<res.x;i++)
		{
			for(int j=0;j<res.y;j++)
				for(int k=0;k<res.z;k++)
				{
					for(int l=0;l<n;l++)
					{
						if(c)
						{
							number re, im;
							in >> re >> im;
							
							d->C->data[i][j][k*n+l] = complex<number>(re,im);
						}
						else
						{
							number re;
							in >> re;

							d->R->data[i][j][k*n+l] = re;
						}
					}
					if(!c && n == 1)
					{
						number val = d->R->data[i][j][k];
						if(first)
						{
							theMin = theMax = val;
							first = false;
						}
						else
						{
							if(val < theMin)
								theMin = val;
							if(val > theMax)
								theMax = val;
						}
					}
				}
				
			cout << '.' << flush;
		}
		cout << "done.\n";
	}
	else
	{
#ifdef STDIO_BIN
		FILE *f = fopen(filename,"rb");
		fseek(f,12,SEEK_SET);
#else
		ifstream in(filename, ios::binary);
		
		in.seekg(12,ios::beg);
#endif
		int cznc = res.z*n * (c?2:1);
		float *theFloats = new float [cznc];
		for(int i=0;i<res.x;i++)
		{
			for(int j=0;j<res.y;j++)
			{
#ifdef STDIO_BIN
				fread(theFloats, sizeof(float), cznc, f);
#else
				in.read((char*) theFloats, sizeof(float)*cznc);
#endif
				if(isLittleEndian)
				{
					for(int k=0;k<cznc;k++)
					{
						unsigned char c,*p = (unsigned char*) (&theFloats[k]);
						c = p[0]; p[0] = p[3]; p[3] = c;
						c = p[1]; p[1] = p[2]; p[2] = c;
					}
				}
				for(int k=0;k<res.z;k++)
				{
					for(int l=0;l<n;l++)
					{
						if(c)
						{
							d->C->data[i][j][k*n+l] = complex<number>(
								theFloats[(k*n+l)*2],theFloats[(k*n+l)*2+1]);
						}
						else
						{
							d->R->data[i][j][k*n+l] = theFloats[k*n+l];
						}
					}
					if(!c && n == 1)
					{
						number val = theFloats[k];
						if(first)
						{
							theMin = theMax = val;
							first = false;
						}
						else
						{
							if(val < theMin)
								theMin = val;
							if(val > theMax)
								theMax = val;
						}
					}
				}
				
			}
			cout << '.' << flush;
		}
		cout << "done.\n";
#ifdef STDIO_BIN
		fclose(f);
#endif
	}
	d->UpdateInfo(theMin, theMax);
	return d;
}

bool ReadEvaluator::AlwaysCreateField()
{
	return true;
}
bool ReadEvaluator::IsComplex()
{
	return c;
}
int ReadEvaluator::Dimensions()
{
	return n;
}
bool ReadEvaluator::GetDefaultResolution(vecN3& res)
{
	res = this->res;
	return true;
}
	

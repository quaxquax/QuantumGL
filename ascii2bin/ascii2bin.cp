#include <iostream>
#include <fstream>
#include <strstream>
#include <string>

using namespace std; 	//introduces namespace std

string GetFileName_Open();
string GetFileName_Save(string def);

const short endianTest = 1;
const bool&	isLittleEndian = (char)*((char*)&endianTest);

static void swapshort(short *s)
{
	if(isLittleEndian)
	{
		unsigned char c,*p = (unsigned char*) s;
		c = p[0];
		p[0] = p[1];
		p[1] = c;
	}
}

int main( int argc, char **argv )
{
	string inname, outname;
	if(argc != 3)
	{
		inname = GetFileName_Open();
		outname = GetFileName_Save(inname + ".bin");
	}
	else
	{
		inname = argv[1];
		outname = argv[2];
	}
	
	/*ifstream in0(argv[1]);
	
	in0.seekg(0,ios::end);
	int nbytes = (in0.tellg().offset());// << endl;
	in0.seekg(0,ios::beg);
	char *buf = new char[nbytes];
	in0.read(buf,nbytes);
	
	istrstream in(buf,nbytes);*/
	ifstream in(inname.c_str());
	
	ofstream out(outname.c_str(),ios::binary);
	
	int c;
	int n;
	int cx, cy, cz;
	in >> c;
	if(c > 1)
	{
		cout << "Invalid complex flag #" << c << " - probably old-style header...\n";
		cx = c;
		cout << "Complex? ";
		cin >> c;
		cout << "Dimensions? ";
		cin >> n;
		in >> cy >> cz;
	}
	else
		in >> n >> cx >> cy >> cz;

	out.put('B');
	out.put('i');
	out.put('n');
	out.put('F');
	if(c)
		out.put('C');
	else
		out.put('R');
	out.put((char)n);
	short tmpS = cx;
	swapshort(&tmpS);
	out.write((char*)&tmpS,2);
	tmpS = cy;
	swapshort(&tmpS);
	out.write((char*)&tmpS,2);
	tmpS = cz;
	swapshort(&tmpS);
	out.write((char*)&tmpS,2);
	
	cout << "         ";
	cout << "[";
	for(int i=0;i<cx;i++)
		cout << " ";
	cout << "]\n";
	cout << "Converting";
	
	int cznc = cz*n * (c?2:1);
	float *theFloats = new float [cznc];
	int idx;
	for(int i=0;i<cx;i++)
	{
		for(int j=0;j<cy;j++)
		{
			idx = 0;
			for(int k=0;k<cznc;k++)
			{				
				in >> theFloats[k];
				
				if(isLittleEndian)
				{
					unsigned char c,*p = (unsigned char*) (&theFloats[k]);
					c = p[0]; p[0] = p[3]; p[3] = c;
					c = p[1]; p[1] = p[2]; p[2] = c;
				}
			}
			out.write((char*) theFloats, sizeof(float)*cznc);
		}	
		cout << '.' << flush;
	}
	cout << "done.\n";
	return 0;
}
#include <string>
#include <iostream>

using namespace std;

string GetFileName_Open()
{
	char buf[1024];
	cout << "Input File: ";
	cin.getline(buf,1024);
	
	return buf;
}

string GetFileName_Save(string def)
{
	char buf[1024];
	cout << "Output File: [" << def << "] ";
	cin.getline(buf,1024);
	
	if(buf[0] == 0)
		return def;
	return buf;
}

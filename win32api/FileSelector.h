/*!
	\file		FileSelector.h
	\author		Roman Weinberger
	\date		07-11-2002
	
	image-export dialog
*/
#include <windows.h>
#include <string>

class FileSelector
{
public:
    FileSelector() : name("c:\\output.png") { w = h = 512; }
    ~FileSelector() { };    
    
    int modal(HWND parent, int maxw, int maxh, bool exp_shot=true);
    BOOL proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
    std::string name;
    int w;
    int h;

private:
    int max_w;
    int max_h;
    bool shot;
};

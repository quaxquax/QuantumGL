/*!
	\file		FileSelector.cpp
	\author		Roman Weinberger
	\date		07-11-2002
	
	image-export dialog
*/

#include <cmath>

#include "FileSelector.h"
#include "qDialog.h"
#include "globals.h"



// dirty but acceptable with modal stuff...
static FileSelector *currSelector=NULL;

// simple redir
static BOOL CALLBACK DlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return currSelector->proc(hwnd, msg, wParam, lParam);
};

// starts modal dialog
int FileSelector::modal(HWND parent, int maxw, int maxh, bool exp_shot)
{
    max_w = maxw;
    max_h = maxh;
    shot = exp_shot;
    currSelector = this;
    int res = DialogBox(hInstance, "qFileDialog", parent, DlgProc);
    currSelector = NULL;
    return res;
};

// message proc
BOOL FileSelector::proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    char buffer[1024];
    switch(msg)
    {
    case WM_INITDIALOG:
        sprintf(buffer, "%s", name.c_str());
        SetDlgItemText(hwnd, IDC_EXPORTNAME, buffer);
        sprintf(buffer, "%d", w);
        SetDlgItemText(hwnd, IDC_EXPORT_WIDTH, buffer);
        sprintf(buffer, "%d", h);
        SetDlgItemText(hwnd, IDC_EXPORT_HEIGHT, buffer);
    break;
    case WM_COMMAND:
            if( LOWORD(wParam) == IDC_EXPORTNAME_CHOOSE )
            {
                char szFile[256];
                ZeroMemory(szFile, 256);
                // get file and open    
                OPENFILENAME ofn;
                ZeroMemory(&ofn, sizeof(OPENFILENAME));
                ofn.lStructSize = sizeof(OPENFILENAME);
                ofn.hwndOwner = hwndMain;
                ofn.lpstrFile = szFile;
                ofn.nMaxFile = sizeof(szFile);
                ofn.lpstrFilter = "Portable Network Graphics (*.png)\0*.png\0All Files (*.*)\0*.*\0";
                ofn.nFilterIndex = 1;
                ofn.lpstrFileTitle = NULL;
                ofn.nMaxFileTitle = 0;
                ofn.lpstrInitialDir = NULL;
                ofn.Flags = OFN_EXPLORER;
    
                
                if( GetSaveFileName(&ofn) ) 
                { 
                    std::string s(szFile);
                    if( shot ) 
                    if( shot && s.npos == s.find(".png") ) s += ".png";
                    SetDlgItemText(hwnd, IDC_EXPORTNAME, s.c_str());     
                }
            }
            else if( wParam == IDOK ) 
            {  
                GetDlgItemText(hwnd, IDC_EXPORTNAME, buffer, 1024);
                name = std::string(buffer);
                GetDlgItemText(hwnd, IDC_EXPORT_WIDTH, buffer, 1024);
                w = atoi(buffer);
                GetDlgItemText(hwnd, IDC_EXPORT_HEIGHT, buffer, 1024);
                h = atoi(buffer);
                
                bool msg=false;
                if( h > max_h || h < 0 ) { h = max_h; msg = true; }
                if( w > max_w || w < 0 ) { w = max_w; msg = true; }
                
                if( msg ) 
                    MessageBox(hwnd, "Your GfxCard does not support this image resolution, the maximum possible will be used...", "Warning", MB_OK);
                
                EndDialog(hwnd, 1);
            }
            else if( wParam == IDCANCEL ) 
            {
                EndDialog(hwnd, 0);
            }
    break;
    };
    return FALSE;
};

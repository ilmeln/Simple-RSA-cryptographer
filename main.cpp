#include <string>
std::string cls =  "My class",wn="Window",f = "fnt";
#include "resourse.h"
#include <stdlib.h>
#include <math.h>
char buff[256];
#include <sstream>
#include <vector>
#include "gpg.h"
HFONT font;
HDC hd;
void setWall(int x,int y);
HINSTANCE winHinst;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK DialogProc( HWND   hwndDlg, UINT   uMsg, WPARAM wParam, LPARAM lParam);
char _x[5],_y[5],_r[5];
INT_PTR CALLBACK DialogProcR( HWND   hwndDlg, UINT   uMsg, WPARAM wParam, LPARAM lParam);
unsigned int char_hight=16,char_lenght=8,win_wight=80,win_hight=25;
HWND handle;
 HWND hEdit;
std::stringstream ss;
std::string getText(HWND h);





int WINAPI WinMain (HINSTANCE hThisInstance,HINSTANCE hPrevInstance,LPSTR lpszArgument,int nCmdShow)
{
    MSG msg;
    winHinst = hThisInstance;
    WNDCLASSEX wcs = WNDCLASSEX();
    wcs.cbSize = sizeof(WNDCLASSEX);
    wcs.lpfnWndProc = WndProc;
    wcs.hInstance = winHinst;
    wcs.hIcon = LoadIcon(NULL,IDI_APPLICATION);
    wcs.hCursor = LoadCursor(NULL,IDC_ARROW);
    wcs.lpszMenuName = MAKEINTRESOURCE(M_MENU);
    wcs.hIconSm = LoadIcon(NULL,IDI_APPLICATION);
    wcs.lpszClassName = cls.c_str();
    if(!RegisterClassEx(&wcs)) {
        return 0;
    }
    DWORD winStyle = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_VISIBLE | CW_USEDEFAULT;
    RECT winSize;
    winSize.left = winSize.top=0;
    winSize.right = win_wight*char_lenght;
    winSize.bottom = win_hight*char_hight;
    AdjustWindowRect(&winSize,winStyle,FALSE);
    int winX = winSize.right-winSize.left;
    int winY = winSize.bottom-winSize.top;
    handle = CreateWindowEx(WS_EX_CLIENTEDGE,cls.c_str(),wn.c_str(),winStyle,200,200,winX,winY,NULL,NULL,winHinst,NULL);
    if(handle == NULL) {
        return 0;
    }
    ShowWindow (handle, nCmdShow);
    font = CreateFont(char_hight,char_lenght,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,PROOF_QUALITY,FF_MODERN,f.c_str());
    hd = GetDC(handle);
    SelectObject(hd, font);
    SetBkColor(hd,BLACK);
    std::string p(win_wight,' ');
    SetTextColor(hd,BLACK);
    for (unsigned int i=0;i<win_hight;++i) {
        TextOut(hd,0,i*char_hight,p.c_str(),p.size());
    }
    ShowWindow(handle,SW_SHOW);
    while (GetMessage (&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}




LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        switch(wParam) {
        case CREATE_KEY:
            if(pgp.generate_keys()) {
                MessageBox(hWnd,"Keys generated","Error",MB_OK);
            }
            break;
        case IMPORT_KEY:
            if(!pgp.import_key()) {
                MessageBox(hWnd,"import.txt Not found\n","Error",MB_OK);
            }
            else {
                MessageBox(hWnd,"Key imported","Succes",MB_OK);
            }
            break;
        case ENCRYPT:{
            auto buffer = getText(hEdit);
            buffer = pgp.encrypt(buffer);
            UpdateWindow(handle);
            SetTextColor(hd,BLACK);
            SetBkColor(hd,WHITE);
            if(buffer.size()>win_wight) {
                int pos=0,len = win_wight-1,ypos=0;
                while(true) {
                    std::string s;
                    if((unsigned int)(pos+len)<buffer.size()-pos) {
                        s = buffer.substr(pos,len);
                        pos=len;
                        len+=len;
                        SetWindowText(hEdit,s.c_str());
                        ++ypos;
                    }
                    else {
                        s = buffer.substr(pos,len);
                        SetWindowText(hEdit,s.c_str());
                        break;
                    }
                }
            }
            else {
                SetWindowText(hEdit,buffer.c_str());
            }
            break;
        }
        case DESC: {
            auto buffer = getText(hEdit);
            buffer = pgp.decrypt(buffer);
            SetWindowText(hEdit,buffer.c_str());
            break;
        }
        default:
            break;
        }
        break;
    case WM_DESTROY:
            PostQuitMessage(0);
            break;
    case WM_CREATE:
            HMENU menu,subMenu;
            hEdit = CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT", "",WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE
                                    | ES_AUTOVSCROLL | ES_AUTOHSCROLL,0, 0, 100, 100, hWnd, (HMENU)IDC_MAIN_EDIT,
                                    GetModuleHandle(NULL), NULL);
            if(hEdit == NULL) {
                    MessageBox(hWnd, "Could not create edit box.", "Error", MB_OK | MB_ICONERROR);
            }
            menu = CreateMenu();
            subMenu = CreatePopupMenu();
            AppendMenu(subMenu,MF_STRING,CREATE_KEY,"&Create key");
            AppendMenu(subMenu,MF_STRING,IMPORT_KEY,"&Import key");
            AppendMenu(menu,MF_STRING|MF_POPUP,(UINT)subMenu,"&Keys");
            subMenu = CreatePopupMenu();
            AppendMenu(subMenu,MF_STRING,ENCRYPT,"&Encrypt");
            AppendMenu(subMenu,MF_STRING,DESC,"&Descrypt");
            AppendMenu(menu,MF_STRING|MF_POPUP,(UINT)subMenu,"&Shiphr");
            SetMenu(hWnd,menu);
            SendMessage(hEdit, WM_SETFONT, (WPARAM)font, MAKELPARAM(FALSE, 0));
            break;
    case WM_SIZE:
            HWND hedit;
            RECT rcClient;
            GetClientRect(hWnd, &rcClient);
            hedit = GetDlgItem(hWnd, IDC_MAIN_EDIT);
            SetWindowPos(hedit, NULL, 0, 0, rcClient.right, rcClient.bottom,SWP_NOZORDER);
            break;
    default:
            return DefWindowProc (hWnd, message, wParam, lParam);
    }
    return 0;
}

std::string getText(HWND h)
{
    int size = GetWindowTextLength(h);
    ++size;
    char *buf = new char[size];
    GetWindowText(h,buf,size);
    std::string s = buf;
    delete[] buf;
    return s;
}

void import()
{
    OPENFILENAME ofn;
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = handle;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
}


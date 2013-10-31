#include "Win32Application.h"

#define IDI_MAIN_ICON ""
#define ID_APP_MAIN   L"Win32Application"

HWND
CreateWin32Window(int _width, int _height, HINSTANCE hInstance)
{    
    WNDCLASS wincl;
    wincl.style         = 0;
	wincl.lpfnWndProc   = ::WindowProc;
    wincl.cbClsExtra    = 0;
    wincl.cbWndExtra    = 0;
    wincl.hInstance     = hInstance;
    wincl.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAIN_ICON));
    wincl.hCursor       = LoadCursor(hInstance, IDC_ARROW);
    wincl.hbrBackground = 0;
    wincl.lpszMenuName  = NULL;
    wincl.lpszClassName = ID_APP_MAIN;

    ::RegisterClass(&wincl);

	int iFullScrWidth = ::GetSystemMetrics( SM_CXFULLSCREEN );
    int iFullScrHeight = ::GetSystemMetrics( SM_CYFULLSCREEN );
    int iGapX = (iFullScrWidth - _width) / 2;
    int iGapY = (iFullScrHeight - _height) / 2;

    RECT rect;
    rect.left = iGapX;
    rect.top = iGapY;
    rect.right = iGapX + _width;
    rect.bottom = iGapY + _height;

    ::AdjustWindowRect(&rect, WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX, 0);

    int iAppX = rect.left;
    int iAppY = rect.top;
    int iAppW = rect.right - rect.left;
    int iAppH = rect.bottom - rect.top;

	HWND hWnd = ::CreateWindow( 
        ID_APP_MAIN, ID_APP_MAIN,
        WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_CLIPCHILDREN, 
        iAppX, iAppY, iAppW, iAppH, 
        NULL, 
        NULL, 
        hInstance, 
        NULL);

    if (NULL == hWnd)
        return false;
	
    ::ShowWindow(hWnd, SW_SHOWNORMAL);
    ::UpdateWindow(hWnd);

    return hWnd;
}

LRESULT CALLBACK WindowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
	//case WM_NCACTIVATE:
		//{
            /*
			if (false == LOWORD(wParam))
			{                         
				LOG("App NC Focus = FALSE");

				THEAPPLICATION()->Flush();
			}
			else
				LOG("App NC Focus = TRUE");
            */
		//}
		//break;
	//case WM_ACTIVATE:
		//{
            /* 
			if (LOWORD(wParam) == WA_INACTIVE)
			{
				LOG("App Focus = FALSE");

				THEAPPLICATION()->Flush();
			}
			else
				LOG("App Focus = TRUE");
            */
		//}
		//break;
	case WM_CLOSE:
        COgreApplication::sharedApplication()->End();
        break;

	case WM_DESTROY:
		PostQuitMessage(0);      
        break;

    default:
        return ::DefWindowProc( hWnd, uMsg, wParam, lParam );
    }

    return 0;
}
#pragma once

#include <windows.h>

HWND CreateWin32Window(int _width, int _height, HINSTANCE hInstance);
static LRESULT CALLBACK WindowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

#include "window.h"

void TWindow::SetupWindow() {
	if (wcex.lpszClassName == NULL) {
		UUID guid;
		UuidCreate(&guid);
		RPC_CSTR szGuid;
		UuidToString(&guid,&szGuid);
		wcex.lpszClassName	= (char*)szGuid;
		ATOM wndClass			= RegisterClassEx(&wcex);
		wcex.lpszClassName = (LPCSTR)wndClass;
	}

	HWindow = CreateWindow( 
		wcex.lpszClassName,
		title,
		windowStyle,
		X,
		Y,
		W,
		H,
		Parent?Parent->HWindow:NULL,
		(HMENU)Id, //menu
		hInstance,
		this
	);
}

#include <map>
LRESULT CALLBACK TWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static std::map<HWND,TWindow*> windowMap;

	if (message == WM_CREATE) {
		TWindow* pTWindow = (TWindow*)((LPCREATESTRUCT)lParam)->lpCreateParams;
		windowMap[hWnd] = pTWindow;
	}
	if (message == WM_PAINT) {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		windowMap[hWnd]->Paint(hdc,ps);
		EndPaint(hWnd, &ps);
		return 0;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

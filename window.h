#pragma once
#include <windows.h>
#include <map>
#include <list>

#ifndef MoveTo
	#define MoveTo(a,b,c) MoveToEx(a,b,c,NULL)
#endif

#define RTMessage MSG
#define ID_FIRST WM_USER
#define CM_FIRST 0
#define WM_FIRST 0
#define CM_EXIT  IDCANCEL


struct TWindow {
	HINSTANCE hInstance;
	TWindow *Parent;
	HWND HWindow;
	const char *title;
	int Id;
	int windowStyle;
	WNDCLASSEX wcex;
	int X,Y,W,H;

	TWindow(TWindow* AParent, const char* ATitle, HINSTANCE AnInstance) {
		hInstance = AnInstance;
		Parent = AParent;
		title = ATitle;
		Id = 0;
		windowStyle = WS_VISIBLE;
		if (AParent != NULL) windowStyle |= WS_CHILD;

		// Register the class
		memset(&wcex,0,sizeof(WNDCLASSEX));
		wcex.cbSize		= sizeof(WNDCLASSEX);
		wcex.style			= CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc	= TWindow::WndProc;
		wcex.cbClsExtra	= 0;
		wcex.cbWndExtra	= 0;
		wcex.hInstance		= hInstance;
		wcex.hIcon			= NULL;
		wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
		wcex.lpszMenuName	= NULL;
		wcex.lpszClassName	= NULL;
		wcex.hIconSm		= NULL;
	}
	void CloseWindow() { PostMessage(HWindow,WM_CLOSE,0,0); }

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	virtual void Paint (HDC PaintDC, PAINTSTRUCT  &PaintInfo) =0;
	virtual void SetupWindow(); //this has to create the window and all children
	virtual void Show(int nCmdShow) { ShowWindow(HWindow,nCmdShow); }
};
typedef TWindow* PTWindowsObject;

//struct TStatic : public TWindow {
//	TStatic(TWindow *AParent,int AnId,const char *title,int x, int y, int w, int h,int unknown) 
//		: TWindow(AParent, title, AParent->hInstance)
//	{
//		wcex.lpszClassName = "STATIC";
//		Id = AnId;
//		X = x;
//		Y = y;
//		W = w;
//		H = h;
//	}
//	void SetText(const char *text) { SendMessage(HWindow,WM_SETTEXT,0,(LPARAM)text); }
//};
//typedef TStatic* PTStatic;

//struct TButton : public TWindow {
//	TButton(TWindow *AParent,int AnId,const char *title,int x, int y, int w, int h,BOOL IsDefault) 
//		: TWindow(AParent, title, AParent->hInstance)
//	{
//		wcex.lpszClassName = "BUTTON";
//		Id = AnId;
//		X = x;
//		Y = y;
//		W = w;
//		H = h;
//		fIsDefault = IsDefault;
//	}
//};
//typedef TButton* PTButton;

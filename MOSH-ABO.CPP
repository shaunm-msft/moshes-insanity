#define _USE_MATH_DEFINES
#include <math.h>
#include <string.h>
#include "moshe.h"
#include "mosh-con.h"

#define IDD_ABOUTCUBE	101
#define IDD_TIMER			1000
#define WIDTH_HEIGHT		25




/*--------------------------------------------------------------*
 * Class to handle about dialog & associated animation.         *
 *--------------------------------------------------------------*/
class TDialogAbout : public TWindow {
	Cube c;
	PTSmallCubeWindow scw;
	int wh;
	TWindow *Parent;

	virtual void SetupWindow ();
	virtual void WMClose (RTMessage Msg);
	virtual void WMTimer (RTMessage Msg);

public:
	virtual void Paint (HDC PaintDC, PAINTSTRUCT  &PaintInfo) {}

	static INT_PTR CALLBACK TDialogAbout::MsgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	TDialogAbout (PTWindowsObject AParent, LPSTR AName) : TWindow(AParent,AName,AParent->hInstance) {	}
	~TDialogAbout () {};
};

/*--------------------------------------------------------------*
 * Handle animation of cube on about dialog box                 *
 * Entry:  Reference to an event message                        *
 * Return: None                                                 *
 *--------------------------------------------------------------*/
void TDialogAbout::WMTimer (RTMessage msg)
{
	BOOL br;
   char szTimer[] = "About Timer";

		// rotate cube 1/10th of a circle around the z-axis
	c.Rotate(0.0,0.0,M_PI/5.0);

		// erase window
	scw->uwnd.Erase();

   	// draw the small cube on the window (offscreen)
	c.Draw(scw->uwnd);

		// copy the small cube onto the main window
	HWND hc = GetDlgItem(msg.hwnd,IDD_ABOUTCUBE);
	HDC dest,src;
	dest = GetDC(hc);
	if (!dest) ((PTMainWindow)Parent)->ReportError(szTimer,1);
	src = scw->uwnd.MemScreenDC();
	if (!src) ((PTMainWindow)Parent)->ReportError(szTimer,2);
	br = BitBlt(dest,0,0,wh,wh, src,0,0, SRCCOPY);
	if (!br) ((PTMainWindow)Parent)->ReportError(szTimer,3);
	br = ReleaseDC(hc,dest);
	if (!dest) ((PTMainWindow)Parent)->ReportError(szTimer,4);
	br = DeleteDC(src);
	if (!dest) ((PTMainWindow)Parent)->ReportError(szTimer,5);

	return;
}

/*--------------------------------------------------------------*
 * Kill dialog box                                              *
 * Entry:  Reference to an event message                        *
 * Return: None                                                 *
 *--------------------------------------------------------------*/
void TDialogAbout::WMClose (RTMessage Msg)
{
	KillTimer(Msg.hwnd,IDD_TIMER);
	delete scw;
}

/*--------------------------------------------------------------*
 * Setup about dialog box                                       *
 * Entry:  None                                                 *
 * Return: None                                                 *
 *--------------------------------------------------------------*/
void TDialogAbout::SetupWindow ()
{
	c.SetEquation(SmallCubeBaseRectP1,SmallCubeBaseRectP2,SmallCubeHeight);
	memmove(&c.offset,&SMALLCUBEOFFSET,sizeof(DPOINT3D));
	c.BorderColor(RGB(0,0,0));
	c.bHiddenFaces = 1;
	c.bColorFaces = 1;
	c.crFace[0] = RGB(0,0,255);
	c.crFace[1] = RGB(0,255,0);
	c.crFace[2] = RGB(0,255,255);
	c.crFace[3] = RGB(255,0,0);
	c.crFace[4] = RGB(255,0,255);
	c.crFace[5] = RGB(255,255,0);

	RECT r;
	GetWindowRect(GetDlgItem(HWindow,IDD_ABOUTCUBE),&r);
	wh = r.right - r.left;
	scw = new TSmallCubeWindow (this,5,5,wh);

	//TDialog::SetupWindow();
	scw->uwnd.Init(scw->HWindow,-1,1,-0.5,1,
		2.5,2.6,&SMALLCUBEVIEWPOINT,FALSE,RGB(0,0,0),GetSysColor(COLOR_WINDOW));

	SetTimer(HWindow,IDD_TIMER,250,NULL);

	return;
}

/*--------------------------------------------------------------*
 * Message proc for the about dialog                            *
 * Entry:  the usual for a dialog message proc                  *
 * Return: the usual for a dialog message proc                  *
 *--------------------------------------------------------------*/
INT_PTR CALLBACK TDialogAbout::MsgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static TDialogAbout *dlg;

	MSG msg;
	msg.hwnd = hDlg;
	msg.lParam = lParam;
	msg.wParam = wParam;
	msg.message = message;
	msg.time = 0;
	POINT pt = {0,0};
	msg.pt = pt;

	switch (message)
	{
	case WM_INITDIALOG:
		dlg = (TDialogAbout*)lParam;
		dlg->HWindow = hDlg;
		dlg->SetupWindow();
		return (INT_PTR)TRUE;

	case WM_TIMER:
		dlg->WMTimer(msg);
		return (INT_PTR)0;

	case WM_CLOSE:
		dlg->WMClose(msg);
		return (INT_PTR)0;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
/*--------------------------------------------------------------*
 * Display about dialog box                                     *
 * Entry:  Event message                                        *
 * Return: None                                                 *
 *--------------------------------------------------------------*/
void TMainWindow::TMainWindowIDM_HELPABOUT (RTMessage)
{
	TDialogAbout *dlgAbout = new TDialogAbout (this,"MosheAbout");
	DialogBoxParam(hInstance,MAKEINTRESOURCE(IDD_MOSHEABOUT),HWindow,&TDialogAbout::MsgProc,(LPARAM)dlgAbout);
}

void TMainWindow::TMainWindowIDM_HELPCONTENTS (RTMessage)
{
	WinHelp(HWindow,"moshe.hlp",HELP_CONTENTS,0);
}
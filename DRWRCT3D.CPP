/********************************************************************
 *                                                                  *
 *   Source File: DrwRct3D.cpp                                      *
 *                                                                  *
 *   by Shaun Miller, 1994                                          *
 *                                                                  *
 ********************************************************************/



#include "drwrct3d.h"


/*--------------------------------------------------------------*
 * Handles WM_PAINT event.  It simply copies the memory bitmap  *
 *   of the display onto the visible screen.                    *
 * Entry:  HDC - DC of window to paint                          *
 *         PAINTSTRUCT reference - info about window to paint   *
 * Return: None                                                 *
 *--------------------------------------------------------------*/
void TDrawRect3D::Paint (HDC PaintDC, PAINTSTRUCT  &PaintInfo)
{
	HDC hdcMemScreen = uwnd.MemScreenDC();

	BitBlt(PaintDC, PaintInfo.rcPaint.left, PaintInfo.rcPaint.top,
		PaintInfo.rcPaint.right-PaintInfo.rcPaint.left,
		PaintInfo.rcPaint.bottom-PaintInfo.rcPaint.top,
		hdcMemScreen, PaintInfo.rcPaint.left, PaintInfo.rcPaint.top,
		SRCCOPY);
	DeleteDC(hdcMemScreen);
	return;
}

/*--------------------------------------------------------------*
 * Constructor.  Sets window position & style.                  *
 * Entry:  Pointer to windows object - parent of this window    *
 *         Integer - id of child                                *
 *         Integer - x coord of window origin within parent     *
 *         Integer - y coord of window origin within parent     *
 *         Integer - width of window                            *
 *         Integer - height of window                           *
 * Return: None                                                 *
 *--------------------------------------------------------------*/
TDrawRect3D::TDrawRect3D(PTWindowsObject AParent, int AnID,
	int X,int Y,int W,int H) : TWindow (AParent,"")
{
	Attr.Style = WS_CHILD | WS_VISIBLE;
	Attr.X = X;
	Attr.Y = Y;
	Attr.W = W;
	Attr.H = H;
	Attr.Id = AnID;
}

/*--------------------------------------------------------------*
 * Handles WM_LBUTTONDOWN event.  Merely passes the message on  *
 *   to its parent window (after mapping the click point to     *
 *   parent window coordinates).                                *
 * Entry:  Event message                                        *
 * Return: None                                                 *
 *--------------------------------------------------------------*/
void TDrawRect3D::WMLButtonDown (RTMessage Msg)
{
	POINT p;
	p.x = LOWORD(Msg.lParam);
	p.y = HIWORD(Msg.lParam);
	MapWindowPoints(HWindow,Parent->HWindow,&p,1);
	Msg.lParam = MAKELPARAM(p.x,p.y);
	PostMessage(Parent->HWindow,Msg.message,Msg.wParam,Msg.lParam);
	return;
}

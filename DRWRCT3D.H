/********************************************************************
 *                                                                  *
 *   Source File: DrwRct3D.h                                        *
 *                                                                  *
 *   by Shaun Miller, 1994                                          *
 *                                                                  *
 ********************************************************************/


#ifndef __DRAWRECT3D__
#define __DRAWRECT3D__

#include "window.h"
#include <windows.h>
#include "usrwnd3d.h"


// Declare TDrawRect3D, a TWindow descendant
// This is the rectangle in which the shapes are drawn.
class TDrawRect3D : public TWindow
{
public:
	UserWindow3D uwnd;

	//virtual void WMLButtonDown (RTMessage Msg);
	//virtual void Paint (HDC PaintDC, PAINTSTRUCT  &PaintInfo);
	//TDrawRect3D(PTWindowsObject AParent, int AnID, int X, int Y, int W, int H)
	TDrawRect3D(TWindow *AParent,int AnId,int x, int y, int w, int h) 
		: TWindow(AParent, "", AParent->hInstance)
	{
		Id = AnId;
		X = x;
		Y = y;
		W = w;
		H = h;
	}
	~TDrawRect3D () {};
	virtual void Paint (HDC PaintDC, PAINTSTRUCT  &PaintInfo) {
#if 1
	HDC hdcLargeCube = uwnd.MemScreenDC();
	BitBlt(PaintDC,0,0,W,H,hdcLargeCube,0,0,SRCCOPY);
	DeleteDC(hdcLargeCube);
	BringWindowToTop(HWindow);
#endif
	}
};
typedef TDrawRect3D* PTDrawRect3D;

#endif

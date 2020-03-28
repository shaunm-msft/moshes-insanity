/********************************************************************
 *                                                                  *
 *   Source File: UserWind.cpp                                      *
 *                                                                  *
 *   This file contains the UserWindow class functions.             *
 *   The UserWindow object handles all translation between the user *
 *     window and the screen.                                       *
 *                                                                  *
 *   by Shaun Miller, 1994                                          *
 *                                                                  *
 ********************************************************************/



#include "userwind.h"
#include "window.h"
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>


/*-----------------------------------------------------------------*
 * Constructor.  Merely zeros out the internal object data.  To    *
 *   use the window, call Init(...).                               *
 * Entry:  None                                                    *
 * Return: None                                                    *
 *-----------------------------------------------------------------*/
UserWindow::UserWindow ()
{
	hwndScreenViewport = NULL;
	memset(&rScreenViewport,'\0',sizeof(RECT));
	memset(&rUserWindow,'\0',sizeof(RECT));
	hbmpMemScreen = NULL;
	ptScreenCoordOfUserWindowOrigin.x = 0;
	ptScreenCoordOfUserWindowOrigin.y = 0;
	cxScreenPixelsPerUserWindowUnit = 0;
	cyScreenPixelsPerUserWindowUnit = 0;
	cxScreenPixels,cyScreenPixels = 0;
	cxUserUnits = 0;
	cyUserUnits = 0;
	crBkGnd = RGB(255,255,255);
	crFgGnd = RGB(0,0,0);
	Status = UW_OK;
	bAxes = TRUE;
	crAxes = RGB(0,0,0);
}

/*-----------------------------------------------------------------*
 * Destructor.  Destroys the bitmap of the memory display.         *
 * Entry:  None                                                    *
 * Return: None                                                    *
 *-----------------------------------------------------------------*/
UserWindow::~UserWindow ()
{
	if (hbmpMemScreen) DeleteObject(hbmpMemScreen);
}

/*-----------------------------------------------------------------*
 * Window initialization (2 functions).  The only difference       *
 *   between the two Init routines is the ability to specify the   *
 *   screen viewport coordinates of the user window origin         *
 *   (defaulted to center of viewport).                            *
 * Entry: Handle to window of screen viewport                      *
 *        Point to RECT - bounding rectangle of user window (must  *
 *                        be whole number coords)                  *
 *        Integer (optional) - whether to show coordinate axes     *
 *                  default TRUE (ie. show axes)                   *
 *        COLORREF (optional) - foreground (border) color          *
 *                              default RGB(0,0,0)                 *
 *        COLORREF(optional) - background color                    *
 *                             default RGB(255,255,255)            *
 *        COLORREF (optional) - axes color, default RGB(0,0,0)     *
 * Return: None                                                    *
 *-----------------------------------------------------------------*/
void UserWindow::Init (HWND hwndScreen,
								RECT *rUserWnd,
								int bShowAxes,
								COLORREF crForeGround,
								COLORREF crBackGround,
								COLORREF crAxesColor)
{
		// Limited parameter validation
	Status = UW_OK;
	if (!IsWindow(hwndScreen) || !rUserWnd) {
		Status = UW_PARAM;
		return;
   }

		// Directly copy the input parameters
		// handle to screen viewport, user window bounding rect,
		// background, foreground, axes colors,
      // whether to show coord axes.
	hwndScreenViewport = hwndScreen;
	memmove(&rUserWindow,rUserWnd,sizeof(RECT));
	SetBkColor(crBackGround);
	SetFgColor(crForeGround);
	SetAxesColor(crAxesColor);
	bAxes = bShowAxes;

		// compute data needed to translate between user units
      // & screen coordinates
	InitCompute();

   	// default origin to center of screen viewport
	ptScreenCoordOfUserWindowOrigin.x = rScreenViewport.left
		+ cxScreenPixels/2;
	ptScreenCoordOfUserWindowOrigin.y = rScreenViewport.top
		+ cyScreenPixels/2;

		// Clear screen & draw axes
	Erase();

	return;
}

/*-----------------------------------------------------------------*
 * Window initialization (2 functions).  The only difference       *
 *   between the two Init routines is the ability to specify the   *
 *   screen viewport coordinates of the user window origin         *
 *   (defaulted to center of viewport).                            *
 * Entry: Handle to window of screen viewport                      *
 *        Point to RECT - bounding rectangle of user window (must  *
 *                        be whole number coords)                  *
 *        POINT - point of window origin (screen coordinates)      *
 *        Integer (optional) - whether to show coordinate axes     *
 *                  default TRUE (ie. show axes)                   *
 *        COLORREF (optional) - foreground (border) color          *
 *                              default RGB(0,0,0)                 *
 *        COLORREF(optional) - background color                    *
 *                             default RGB(255,255,255)            *
 *        COLORREF (optional) - axes color, default RGB(0,0,0)     *
 * Return: None                                                    *
 *-----------------------------------------------------------------*/
void UserWindow::Init (HWND hwndScreen,
								RECT *rUserWnd,
                        POINT *ptOriginCoord,
								int bShowAxes,
								COLORREF crForeGround,
								COLORREF crBackGround,
								COLORREF crAxesColor)
{
		// Limited parameter validation
	Status = UW_OK;
	if (!IsWindow(hwndScreen) || !rUserWnd) {
		Status = UW_PARAM;
		return;
   }

		// Directly copy the input parameters
		// handle to screen viewport, user window bounding rect,
		// background, foreground, axes colors,
		// whether to show coord axes,
      // coord of window origin
	hwndScreenViewport = hwndScreen;
	memmove(&rUserWindow,rUserWnd,sizeof(RECT));
	SetBkColor(crBackGround);
	SetFgColor(crForeGround);
	SetAxesColor(crAxesColor);
	bAxes = bShowAxes;
	memmove(&ptScreenCoordOfUserWindowOrigin,ptOriginCoord,sizeof(POINT));

		// compute data needed to translate between user units
      // & screen coordinates
	InitCompute();

		// Clear screen & draw axes
	Erase();

   return;
}

/*-----------------------------------------------------------------*
 * Initializes the user window object.  Assumes all inputs have    *
 *   been filled correctly.  Called from one of the Init(...)      *
 *   routines.  Computes data needed to translate between user     *
 *   units and screen coordinates.                                 *
 * Entry: None                                                     *
 * Return: None                                                    *
 *-----------------------------------------------------------------*/
void UserWindow::InitCompute ()
{
	if (Status != UW_OK) return;

		// Compute boundaries of screen viewport
	GetClientRect(hwndScreenViewport,&rScreenViewport);

   	// compute # of pixels across screen viewport
	cxScreenPixels = rScreenViewport.right-rScreenViewport.left;
	cyScreenPixels = rScreenViewport.bottom-rScreenViewport.top;

   	// compute # of units across user window 
	cxUserUnits = fabs((double)rUserWindow.right-rUserWindow.left);
	cyUserUnits = fabs((double)rUserWindow.bottom-rUserWindow.top);

   	// compute # of screen pixels pre window unit
	cxScreenPixelsPerUserWindowUnit = cxScreenPixels/cxUserUnits;
	cyScreenPixelsPerUserWindowUnit = cyScreenPixels/cyUserUnits;

		// Create a duplicate of the screen viewport in memory
	if (!hbmpMemScreen) {
		HDC hdcScreen = GetDC(hwndScreenViewport);
		hbmpMemScreen = CreateCompatibleBitmap(hdcScreen,cxScreenPixels,
			cyScreenPixels);
		ReleaseDC(hwndScreenViewport,hdcScreen);
	}
	if (!hbmpMemScreen) {
		Status = UW_MEMORY;
		return;
   }

	return;
}

/*-----------------------------------------------------------------*
 * Clears the screen viewport (with the background color) and      *
 *   redraws the axes.                                             *
 * Entry: None                                                     *
 * Return: None                                                    *
 *-----------------------------------------------------------------*/
void UserWindow::Erase ()
{
	if (Status != UW_OK) return;

		// Erase screen
   HDC hdcMemScreen = MemScreenDC();
	HBRUSH hbrBkGnd = CreateSolidBrush(crBkGnd);
	SelectObject(hdcMemScreen,hbrBkGnd);
	PatBlt(hdcMemScreen,0,0,cxScreenPixels,cyScreenPixels,PATCOPY);
	DeleteDC(hdcMemScreen);
	DeleteObject(hbrBkGnd);

		// Draw axes 
	if (bAxes) DrawAxes();

   	// refresh screen
   Refresh();

	return;
}

/*-----------------------------------------------------------------*
 * Set axes color & redraw axes.                                   *
 * Entry: COLORREF - axes color                                    *
 *        Integer (optional) - whether to redraw the axes now.     *
 *                             default FALSE                       *
 * Return: None                                                    *
 *-----------------------------------------------------------------*/
void UserWindow::SetAxesColor (const COLORREF color, const int bRedraw)
{
	if (Status != UW_OK) return;

	crAxes = color;

	if (bRedraw) DrawAxes();
	return;
}

/*-----------------------------------------------------------------*
 * Retrieve user window rectange structure.                        *
 * Entry: Pointer to RECT - structure to receive window structure  *
 * Return: None                                                    *
 *-----------------------------------------------------------------*/
void UserWindow::GetUserWindowRect (RECT *r) const
{
	if (r) CopyRect(r,&rUserWindow);
	return;
}

/*-----------------------------------------------------------------*
 * Retrieve screen viewport bounding rectangle                     *
 * Entry: Pointer to RECT - structure to receive viewport struct   *
 * Return: None                                                    *
 *-----------------------------------------------------------------*/
void UserWindow::GetScreenViewportRect (RECT *r) const
{
	if (r) CopyRect(r,&rScreenViewport);
	return;
}

/*-----------------------------------------------------------------*
 * Translate a user window point to a screen viewport point.       *
 * Entry: POINT reference - destination viewport point structure   *
 *        DPOINT reference - source user window point structure    *
 * Return: None                                                    *
 *-----------------------------------------------------------------*/
void UserWindow::TranslateToScreenPoint (POINT &ptScreen,
	const DPOINT &ptUser) const
{
	if (Status != UW_OK) return;

	ptScreen.x = ptScreenCoordOfUserWindowOrigin.x
		+ (LONG)(cxScreenPixelsPerUserWindowUnit*ptUser.x);
	ptScreen.y = ptScreenCoordOfUserWindowOrigin.y
		- (LONG)(cyScreenPixelsPerUserWindowUnit*ptUser.y);
		
	return;
}

/*-----------------------------------------------------------------*
 * Translate a screen viewport point to a user window point.       *
 * Entry: DPOINT reference - destination user window point struct  *
 *        POINT reference - source viewport point structure        *
 * Return: None                                                    *
 *-----------------------------------------------------------------*/
void UserWindow::TranslateToUserUnit (DPOINT &ptUser,
	const POINT &ptScreen) const
{
	ptUser.x = (ptScreen.x - ptScreenCoordOfUserWindowOrigin.x)
		/ (double)cxScreenPixelsPerUserWindowUnit;
	ptUser.y = (ptScreen.y - ptScreenCoordOfUserWindowOrigin.y)
		/ (double)-cyScreenPixelsPerUserWindowUnit;
	return;
}

/*-----------------------------------------------------------------*
 * Sets a pixel on the screen viewport (window).                   *
 * Entry: DPOINT reference - point to set                          *
 *        Integer (optional) - update display now (don't wait for  *
 *                             WM_PAINT event)  default TRUE       *
 * Return: None                                                    *
 *-----------------------------------------------------------------*/
void UserWindow::SetPixel (const DPOINT &ptUser, const int bUpdate)
{
	POINT ptScreen;
   RECT r;

	if (Status != UW_OK) return;

	HDC hdcMemScreen = MemScreenDC();

   	// translate point to screen viewport coordinates
	TranslateToScreenPoint(ptScreen,ptUser);

   	// set pixel on screen viewport
	::SetPixel(hdcMemScreen,ptScreen.x,ptScreen.y,crFgGnd);

   	// refresh the portion of screen modified
	if (bUpdate) {
		r.left = ptScreen.x-1;
		r.right = ptScreen.x+1;
		r.top = ptScreen.y-1;
		r.bottom = ptScreen.y+1;
		InvalidateRect(hwndScreenViewport,&r,FALSE);
	}
   	// cleanup
	DeleteDC(hdcMemScreen);
	return;
}

/*-----------------------------------------------------------------*
 * Refresh the display (ie. generate a WM_PAINT event).            *
 * Entry: Integer (optional) - whether to wait for the WM_PAINT    *
 *                             event to refresh the screen or      *
 *                             force the update now.               *
 *                             default TRUE (ie. wait for WM_PAINT)*
 * Return: None                                                    *
 *-----------------------------------------------------------------*/
void UserWindow::Refresh (const int bWaitForWMPaint)
{
#if 1
	InvalidateRect(hwndScreenViewport,&rScreenViewport,FALSE);
	if (!bWaitForWMPaint) UpdateWindow(hwndScreenViewport);
#else
	UpdateWindow(hwndScreenViewport);
#endif
	return;
}

/*-----------------------------------------------------------------*
 * Draw the coordinate axes on the display (screen viewport).      *
 * Entry: None                                                     *
 * Return: None                                                    *
 *-----------------------------------------------------------------*/
void UserWindow::DrawAxes ()
{
	HDC hdcMemScreen = MemScreenDC();
	HBRUSH hbrAxes = CreateSolidBrush(crAxes);
	SelectObject(hdcMemScreen,hbrAxes);
	MoveTo(hdcMemScreen,ptScreenCoordOfUserWindowOrigin.x,
		rScreenViewport.top);
	LineTo(hdcMemScreen,ptScreenCoordOfUserWindowOrigin.x,
		rScreenViewport.bottom);
	MoveTo(hdcMemScreen,rScreenViewport.left,
		ptScreenCoordOfUserWindowOrigin.y);
	LineTo(hdcMemScreen,rScreenViewport.right,
		ptScreenCoordOfUserWindowOrigin.y);
	DeleteDC(hdcMemScreen);
	DeleteObject(hbrAxes);
}

/*-----------------------------------------------------------------*
 * Create device context for the offscreen image of the display    *
 *   window.  The caller is responsible to call DeleteDC to        *
 *   destroy the DC when finished using it.                        *
 * Entry: None                                                     *
 * Return: HDC - device context for offscreen image                *
 *-----------------------------------------------------------------*/
HDC UserWindow::MemScreenDC ()
{
	HDC hdcScreen = GetDC(hwndScreenViewport);
	HDC hdcMem = CreateCompatibleDC(hdcScreen);
	ReleaseDC(hwndScreenViewport,hdcScreen);
	SelectObject(hdcMem,hbmpMemScreen);
	return hdcMem;
}

/*-----------------------------------------------------------------*
 * Changes the user window rectangle.                              *
 * Note that this will cause many fields in the internal data      *
 *   structure to be modified, but does not affect the size of     *
 *   the display.  Even so, the display will be erased because     *
 *   we don't have any way of matching the current display to      *
 *   the new size and bounds of the user rectangle.                *
 * Entry: RECT reference - new user window rectangle               *
 * Return: None                                                    *
 *-----------------------------------------------------------------*/
void UserWindow::SetUserWindowRect (const RECT &r)
{
      // save new bounding rectangle for user window
	memmove(&rUserWindow,&r,sizeof(RECT));

		// validate new bounding rectangle
	if (r.left>0 && r.right>0) rUserWindow.left=0;
	if (r.left<0 && r.right<0) rUserWindow.right=0;
	if (r.top>0 && r.bottom>0) rUserWindow.bottom=0;
   if (r.top<0 && r.bottom<0) rUserWindow.top=0;

		// compute # of units across user window
	cxUserUnits = fabs((double)rUserWindow.right-rUserWindow.left);
	cyUserUnits = fabs((double)rUserWindow.bottom-rUserWindow.top);

   	// compute # of screen pixels pre window unit
	cxScreenPixelsPerUserWindowUnit = cxScreenPixels/cxUserUnits;
	cyScreenPixelsPerUserWindowUnit = cyScreenPixels/cyUserUnits;

		// compute new screen coord of user window origin
	ptScreenCoordOfUserWindowOrigin.x =
		rScreenViewport.left
		+ abs(rUserWindow.left)*(LONG)cxScreenPixelsPerUserWindowUnit; 
	ptScreenCoordOfUserWindowOrigin.y =
		rScreenViewport.top
		+ abs(rUserWindow.top)*(LONG)cyScreenPixelsPerUserWindowUnit; 

	Erase();

	return;
}

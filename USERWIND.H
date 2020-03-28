/********************************************************************
 *                                                                  *
 *   Source File: UserWind.h                                        *
 *                                                                  *
 *   This file contains the UserWindow class header                 *
 *   The UserWindow object handles all translation between the user *
 *   window and the screen.                                         *
 *                                                                  *
 *   by Shaun Miller, 1994                                          *
 *                                                                  *
 ********************************************************************/


#ifndef __USERWIND__

#define __USERWIND__
#include <windows.h>

// declaration for a double precision point structure
typedef struct {
	double x,y;
} DPOINT;


class UserWindow {
	private:
		HWND hwndScreenViewport;    // handle of screen viewport window
		RECT rScreenViewport;       // bounding rectangle of screen viewport window
		RECT rUserWindow;           // bounding rectangle of user (imaginary) window
		HBITMAP hbmpMemScreen;      // handle to bitmap image of viewport window
			// screen coord of user window origin
		POINT ptScreenCoordOfUserWindowOrigin;
			// # of horiz. pixels per user window unit
		double cxScreenPixelsPerUserWindowUnit;
			// # of vert. pixels per user window unit
		double cyScreenPixelsPerUserWindowUnit;
			// # of horiz & vert pixels on screen viewport
		int cxScreenPixels,cyScreenPixels;
			// # of horiz & vert units on user window
		double cxUserUnits,cyUserUnits;             
		COLORREF crBkGnd;           // fill color
		COLORREF crFgGnd;           // draw color
		COLORREF crAxes;            // axes color
		int bAxes;                  // boolean:  show axes
		int Status;                 // holds class error status
		void InitCompute();         // compute data needed to translate
      									 // between user units & screen coords

	public:
		enum {UW_OK=0, UW_MEMORY=-999, UW_PARAM};   // Status codes

			// See the .cpp file for descriptions if it isn't clear here!

         // get & set colors
		COLORREF GetAxesColor () const {return crAxes;};
		void SetAxesColor (const COLORREF color, const int bRedraw=FALSE);
		void SetFgColor (const COLORREF fg) {crFgGnd = fg;};
		COLORREF GetFgColor () const {return crFgGnd;};
		void SetBkColor (const COLORREF bk) {crBkGnd = bk;};
		COLORREF GetBkColor () const {return crBkGnd;};

      	// info functions
		int GetStatus () const {return Status;};
		void GetUserWindowRect (RECT *r) const;
		void GetScreenViewportRect (RECT *r) const;
		HDC MemScreenDC();
		HWND Hwnd (void) const {return hwndScreenViewport;};

   	void SetUserWindowRect (const RECT &r);

		void TranslateToScreenPoint (POINT &ptScreen, const DPOINT &ptUser) const;
		void TranslateToUserUnit (DPOINT &ptUser, const POINT &ptScreen) const;

		void SetPixel (const DPOINT &ptUser, const int bUpdate=TRUE);
			// draws axes on screen viewport
		virtual void DrawAxes();    
		void Erase ();
		void Refresh (const int bWaitForWMPaint=TRUE);

		void Init (
			HWND hwndScreenViewport,
			RECT *rUserWindow,
         int bShowAxes = TRUE,
			COLORREF crForeGround = RGB(0,0,0),
			COLORREF crBackGround = RGB(255,255,255),
			COLORREF crAxesColor = RGB(0,0,0)
		);
		void Init (
			HWND hwndScreenViewport,
			RECT *rUserWindow,
			POINT *ptScreenCoordOfUserWindowOrigin,
			int bShowAxes = TRUE,
			COLORREF crForeGround = RGB(0,0,0),
			COLORREF crBackGround = RGB(255,255,255),
			COLORREF crAxesColor = RGB(0,0,0)
		);

		UserWindow ();
		~UserWindow ();
};
#endif
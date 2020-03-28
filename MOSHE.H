/********************************************************************
 *                                                                  *
 *   Source File: Moshe.h                                           *
 *   C++ header file for Moshe application                          *
 *                                                                  *
 *   by Shaun Miller, 1994                                          *
 *                                                                  *
 ********************************************************************/



#ifndef __MOSHE__
#define __MOSHE__

#include <windows.h>
#include "window.h"
#include "usrwnd3d.h"
#include "drwrct3d.h"
#include "cube.h"
#include "dlist.h"
#include "steplang.h"

#ifndef min
	#define min(a,b)		(((a) < (b)) ? (a) : (b))
#endif
#ifndef max
	#define max(a,b)		(((a) > (b)) ? (a) : (b))
#endif

#define IDD_MOSHEABOUT       9999

// IDs for window controls
#define IDW_BTNROTATELEFT    101
#define IDW_BTNROTATEDOWN    102
#define IDW_BTNROTATERIGHT   103
#define IDW_BTNROTATEUP      104
#define IDW_BTNROTATECORNER  105
#define IDW_BTNSOLVE         106
#define IDW_STATUS           107
#define IDW_TIMER            108
#define IDW_BTNRESETGAME     109
#define IDM_MAINMENU        200
#define IDM_GAMENEW          201
#define IDM_GAMERESET        202
#define IDM_GAMESOLVE        203
#define IDM_HELPABOUT		  301
#define IDM_HELPCONTENTS	  302

// various constants related to nbr, position, etc. of cubes
static const int LARGECUBE = 8;
static const int NBRSMALLCUBES = 8;
static const int NBRCUBEPOINTS = 8;
static const int NBRFACES = 6;

#define SETTEXT(text) SendMessage(hStatus,WM_SETTEXT,0,(LPARAM)text)

// TSmallCubeWindow, a TWindow descendant
// this is an offscreen window for drawing the small cubes
// this is all there is to it
class TSmallCubeWindow : public TWindow
{
public:
	UserWindow3D uwnd;
	TSmallCubeWindow (PTWindowsObject AParent,int x, int y, int wh)
		: TWindow (AParent,"", AParent->hInstance)
		{
			X = x;
			Y = y;
			H = W = wh;
		}
	virtual void Paint (HDC PaintDC, PAINTSTRUCT  &PaintInfo) {
		HDC hdcLargeCube = uwnd.MemScreenDC();
		BitBlt(PaintDC,0,0,W,H,hdcLargeCube,0,0,SRCCOPY);
		DeleteDC(hdcLargeCube);
	}
};
typedef TSmallCubeWindow* PTSmallCubeWindow;

// Declare TMainWindow, a TWindow descendant
// Just a container for the other controls
class TMainWindow : public TWindow
{
private:
	HWND hStatus;
		// large cube drawing area
	RECT rlcDraw;
		// large cube
	Cube lc;
   	// large cube width/height on the screen
	int lcwh;
   	// which small cube occupies the ith point on the large cube
   int occupied[NBRSMALLCUBES];
		// wire cube, 8 of which compose the large cube
	Cube wire;

		// index to currently selected cube
		//    0..7 are the small colored cubes
		//    >= 8 is the large cube
	int ics;

		// area of status line
	RECT rStatus;
		// whether we are in the process of animating the solution
	int bAnimatingSolution;
   	// current stage of animation
	int stage;
		// number of stages of animation
	int nbrstages;
		// radians to rotate the currently selected cube (ics)
	double xrad,yrad,zrad;
   	// list of steps to animate solution
	DoubleList<StepLanguage> steplist;

   	// colored small cubes
	Cube sc[NBRSMALLCUBES];
		// colored small cubes location & size on screen
	RECT screct[NBRSMALLCUBES];
   	// colored small cubes width/height on the screen
	int scwh;

		// mouse position when button is pressed
	POINT mpos;
		// current mouse position
	POINT cmpos;

		// device contexts and assoc. bitmaps used in dragging //
      // copy of entire screen display
	HDC hdcMain;
	HBITMAP hbmpMain;
		// background behind small cube being dragged
	HDC hdcBackground;
	HBITMAP hbmpBackground;

		// whether mouse button is down
	BOOL bLButtonDown;
   	// whether mouse has moved
	BOOL bMouseMoved;

		// gray cube used to mark the drop location of the small
      // colored cube into the large cube
	Cube black;
   	// corner of large cube that has black cube
	int masterindex;

public:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	// large cube & offscreen copy of large cube
	PTDrawRect3D pLargeCube, pLC2;
   	// small cube offscreen
	PTSmallCubeWindow pSmallCube;
   	// status line
	//PTStatic pStatus;

		// response to buttons for various ways to rotate the
      // currently selected cube
	void TMainWindowIDW_BTNROTATELEFT (RTMessage Msg);
		//= [ID_FIRST + IDW_BTNROTATELEFT];
	void TMainWindowIDW_BTNROTATEDOWN (RTMessage Msg);
		//= [ID_FIRST + IDW_BTNROTATEDOWN];
	void TMainWindowIDW_BTNROTATERIGHT (RTMessage Msg);
		//= [ID_FIRST + IDW_BTNROTATERIGHT];
	void TMainWindowIDW_BTNROTATEUP (RTMessage Msg);
		//= [ID_FIRST + IDW_BTNROTATEUP];
	void TMainWindowIDW_BTNROTATECORNER (RTMessage Msg);
		//= [ID_FIRST + IDW_BTNROTATECORNER];

      // response to selections from the Game menu
	void TMainWindowIDM_GAMESOLVE (RTMessage Msg);
		//= [CM_FIRST + IDM_GAMESOLVE];
	void TMainWindowIDM_GAMERESET (RTMessage Msg);
		//= [CM_FIRST + IDM_GAMERESET];
	void TMainWindowIDM_GAMENEW (RTMessage Msg);
		//= [CM_FIRST + IDM_GAMENEW];

		// response to selections from the Help menu
	void TMainWindowIDM_HELPABOUT (RTMessage Msg);
		//= [CM_FIRST + IDM_HELPABOUT];
	void TMainWindowIDM_HELPCONTENTS (RTMessage Msg);
		//= [CM_FIRST + IDM_HELPCONTENTS];

      // click & drag events
	virtual void WMLButtonDown (RTMessage Msg);
		//= [WM_FIRST + WM_LBUTTONDOWN];
	virtual void WMLButtonUp (RTMessage Msg);
		//= [WM_FIRST + WM_LBUTTONUP];
	virtual void WMMouseMove (RTMessage Msg);
		//= [WM_FIRST + WM_MOUSEMOVE];

      // animation for rotation, solution
	void TMainWindowIDW_TIMER (RTMessage Msg);
		//= [WM_FIRST + WM_TIMER];

	void AnimatedRotateCube (void);
	void AnimatedMoveCube (int);

      // handles WM_PAINT event for main window
	void Paint (HDC PaintDC, PAINTSTRUCT  &LPPaintInfo);

   	// draws the large & small cubes
	void DrawLargeCube (UserWindow3D &u);
	void DrawSmallCube (int, PAINTSTRUCT *LPPaintInfo = NULL);

   	// select/deselect cube
	void SelectCube (int index, int deselect=FALSE);

   	// whether a specified small cube is in the large cube
	int IsInLargeCube (int index);

   	// whether the puzzle is solved
	int IsSolution (void);

		// sorts the 8 points of the large cube in the order in
      // which they are encountered from the viewpoint
	void SortLargeCube (UserWindow3D &u, int *order);

		// facilitates the animation of rotation of the
      // currently selected cube
	void AnimateRotation(int NumberOfStages);

   	// color small cubes with colors in external color array
	void ColorCubes (void);

		// setup the controls, cubes, etc. on the main window
	virtual void SetupWindow ();

   	// setup menu on main window
	TMainWindow (PTWindowsObject AParent, LPSTR ATitle, HINSTANCE AnInstance);

   	// destroy child controls, windows, etc.
	~TMainWindow ();

		// functions for moving & dropping a small cube
	void DropCube (void);
	void MoveCube (RTMessage);

		// non-fatal error reporting
	void ReportError (const char *FunctionName, const int id);
	int nbr_errors;
	int FatalError;
};
typedef TMainWindow* PTMainWindow;

#endif

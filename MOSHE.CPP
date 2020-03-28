/********************************************************************
*                                                                  *
*   Source File: Moshe.cpp                                         *
*   C++ Source file for Moshe application                          *
*                                                                  *
*   by Shaun Miller, 1994                                          *
*                                                                  *
********************************************************************/


// our coordinate system is (from the viewpoint):
//         z
//         |
//         |
//         |
//         o-----  -y
//        /
//       /
//      /
//    -x


#include <stdlib.h>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "moshe.h"

extern DPOINT3D SMALLCUBEOFFSET = {-1,-1,0.45};
extern DPOINT3D SMALLCUBEVIEWPOINT = {-7,-7,3};
extern DPOINT3D LARGECUBEOFFSET = {1,0.5,-0.2};
extern DPOINT3D LARGECUBEVIEWPOINT = {-9,-5,2.5};
extern DPOINT3D SmallCubeBaseRectP1 = {0,0,0};
extern DPOINT3D SmallCubeBaseRectP2 = {1,1,0};
extern double SmallCubeHeight = 1.0;
extern DPOINT3D LargeCubeBaseRectP1 = {0,0,0};
extern DPOINT3D LargeCubeBaseRectP2 = {2,2,0};
extern double LargeCubeHeight = 2.0;

// Thinking message for status line
char ThinkingStatus[] = "Thinking...";
// Starting message for status line
char StartStatus[] = "Press a button to start!";
// Won game message (used in mosh-drg.cpp)
char WinStatus[] = "You win.";
// Animating solution message for status line
char AnimateSolutionStatus[] = "Solving...";
char TimerErrorMsg[] = "There are no timers available!\n"
"Please close a running application before continuing.";
char AppName[] = "Moshe's Insanity";

// function names for error reporting
char szMoveCube[] = "MoveCube";
char szDropCube[] = "DropCube";

extern char cube[8][6];
extern char corner[8][8][3];    // Left,Right,Top
extern int bottom[4][3];        // cube,corner,rotation
extern int top[4][3];
void firstgame (void);
void nextgame (void);


/***************************************************
* TMosheApp method implementations:               *
*                                                 *
* This is overhead and is not important.          * 
***************************************************/

LRESULT CALLBACK TMainWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	static TMainWindow *mainWindow;

	MSG msg;
	msg.hwnd = hWnd;
	msg.lParam = lParam;
	msg.wParam = wParam;
	msg.message = message;
	msg.time = 0;
	POINT pt = {0,0};
	msg.pt = pt;

	switch (message)
	{
	case WM_FIRST + WM_LBUTTONDOWN:
		mainWindow->WMLButtonDown(msg);
		break;
	case WM_FIRST + WM_LBUTTONUP:
		mainWindow->WMLButtonUp(msg);
		break;
	case WM_FIRST + WM_MOUSEMOVE:
		mainWindow->WMMouseMove(msg);
		break;
	case WM_FIRST + WM_TIMER:
		mainWindow->TMainWindowIDW_TIMER(msg);
		break;

	case WM_CREATE:
		mainWindow = (TMainWindow*)((LPCREATESTRUCT)lParam)->lpCreateParams;
		//mainWindow->HWindow = hWnd;
		//mainWindow->SetupWindow();
		return DefWindowProc(hWnd, message, wParam, lParam);
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		if (wmEvent == BN_CLICKED) switch (wmId)
		{
		case IDW_BTNROTATELEFT:
			mainWindow->TMainWindowIDW_BTNROTATELEFT(msg);
			break;
		case IDW_BTNROTATEDOWN:
			mainWindow->TMainWindowIDW_BTNROTATEDOWN(msg);
			break;
		case IDW_BTNROTATERIGHT:
			mainWindow->TMainWindowIDW_BTNROTATERIGHT(msg);
			break;
		case IDW_BTNROTATEUP:
			mainWindow->TMainWindowIDW_BTNROTATEUP(msg);
			break;
		case IDW_BTNROTATECORNER:
			mainWindow->TMainWindowIDW_BTNROTATECORNER(msg);
			break;
		case IDM_GAMESOLVE:
			mainWindow->TMainWindowIDM_GAMESOLVE(msg);
			break;
		case IDM_GAMERESET:
			mainWindow->TMainWindowIDM_GAMERESET(msg);
			break;
		case IDM_GAMENEW:
			mainWindow->TMainWindowIDM_GAMENEW(msg);
			break;
		case IDM_HELPABOUT:
			mainWindow->TMainWindowIDM_HELPABOUT(msg);
			break;
		case IDM_HELPCONTENTS:
			mainWindow->TMainWindowIDM_HELPCONTENTS(msg);
			break;
		case CM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		mainWindow->Paint(hdc,ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Main program
extern "C"
int APIENTRY WinMain(HINSTANCE hInstance,
					 HINSTANCE hPrevInstance,
					 LPTSTR    lpCmdLine,
					 int       nCmdShow)
{
	TMainWindow *mainWindow = new TMainWindow(NULL, AppName, hInstance);

	//HWND hWnd = CreateWindow((LPCSTR)wndClass, AppName, WS_OVERLAPPEDWINDOW,
	//	CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, MainWindow);
	mainWindow->SetupWindow();

	if (!mainWindow->HWindow)
	{
		return 0;
	}

	ShowWindow(mainWindow->HWindow, nCmdShow);
	UpdateWindow(mainWindow->HWindow);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int) msg.wParam;
}




/****************************************************
* TMainWindow implementations:                     *
****************************************************/

/*--------------------------------------------------------------*
* Facilitates the animation of the rotation of the currently   *
*   selected cube.  Performs a non-animated rotation if no     *
*   timers are available.                                      *
* Assumes:  xrad, yrad, zrad (TMainWindow private variables)   *
*   have been properly set.                                    *
* Entry:  Integer number of intermediate steps to the final    *
*            rotation degrees.                                 *
* Return: None                                                 *
*--------------------------------------------------------------*/
void TMainWindow::AnimateRotation(int NumberOfStages)
{
	// start stage 0
	stage = 0;
	// set # of stages to completion
	nbrstages = NumberOfStages;
	// set timer for every 1/10th second
	int a = SetTimer(HWindow,IDW_TIMER,100,NULL);
	// if no timers available, perform rotation without animation
	if (!a) {
		MessageBox(HWindow, TimerErrorMsg, AppName, MB_ICONEXCLAMATION);
		if (ics<NBRSMALLCUBES) {
			sc[ics].Rotate(xrad,0,0);
			sc[ics].Rotate(0,yrad,0);
			sc[ics].Rotate(0,0,zrad);
		}
		else {
			lc.Rotate(xrad,0,0);
			lc.Rotate(0,yrad,0);
			lc.Rotate(0,0,zrad);
		}
	}
	return;
}

/*-----------------------------------------------------------------*
* This routine is actually a slave routine for the timer.         *
* Performs the requested animation of rotation by rotating        *
*   on a timer the requested radians divided by the number of     *
*   animation stages; first in the x, then y, then z directions.  *
* Assumes:  xrad, yrad, zrad,stage,nbrstages                      *
*   (all TMainWindow private variables) have been properly set.   *
* Entry:  None                                                    *
* Return: None                                                    *
*-----------------------------------------------------------------*/
void TMainWindow::AnimatedRotateCube (void)
{
	// radians to rotate per timer event
	//  0 = x, 1 = y, 2 = z
	static double rad[3];

	// index into rad[].  keeps track of whether we are rotating
	// in the x, y, or z direction
	static int i=0;

	// compute the number of radians to rotate per timer event
	// every time we start the animation.  note, we only
	// rotate in one direction at a time, depending on i
	if (!stage) {
		rad[0]=rad[1]=rad[2]=0.0;
		do {
			switch (i) {
				case 0: rad[0] = xrad/nbrstages; break;
				case 1: rad[1] = yrad/nbrstages; break;
				case 2: rad[2] = zrad/nbrstages; break;
			}
			if (rad[i] != 0.0) break;
			i++;
		} while (i<3);
		if (i>2) stage=nbrstages;
	}
	// when we reach the number of stages of completion in the
	// animation, stop.  or at least go to the next direction
	// to rotate, depending on i.
	if (++stage>nbrstages) {
		if (++i<3) {
			stage = 0;
			AnimatedRotateCube();
			return;
		}
		i=0;
		EnableWindow(HWindow,TRUE);
		return;
	}
	// rotate the currently selected cube & draw!
	// for the small cubes, it is that simple
	// for the large cube, we need to not only rotate the large
	// cube, but also the wire cube which is used to draw the
	// frame of the large cube and any small colored cubes which
	// may be in the large cube
	if (ics<LARGECUBE) {
		sc[ics].Rotate(rad[0],rad[1],rad[2]);
		DrawSmallCube(ics);
	}
	else {
		lc.Rotate(rad[0],rad[1],rad[2]);
		wire.Rotate(rad[0],rad[1],rad[2]);
		for (int j=0; j<NBRSMALLCUBES; j++) if (occupied[j]<LARGECUBE) {
			sc[occupied[j]].Rotate(rad[0],rad[1],rad[2]);
		}
		//wire.Draw(pLargeCube->uwnd);
		DrawLargeCube(pLargeCube->uwnd);
		InvalidateRect(pLargeCube->HWindow,NULL,FALSE);
		UpdateWindow(pLargeCube->HWindow);
	}

	return;
}

/*-----------------------------------------------------------------*
* Resets the current game.  Merely empties the large cube and     *
*   returns the small colored cubes to their original resting     *
*   place, but not necessarily in their original orientation      *
* Entry:  Event message                                           *
* Return: None                                                    *
*-----------------------------------------------------------------*/
void TMainWindow::TMainWindowIDM_GAMERESET (RTMessage Msg)
{
	if (bAnimatingSolution) return;

	SETTEXT(StartStatus);
	for (int i=0; i<NBRSMALLCUBES; i++) {
		occupied[i]=NBRSMALLCUBES;
		memmove(&sc[i].offset,&SMALLCUBEOFFSET,sizeof(DPOINT3D));
	}
	SelectCube(ics,TRUE);
	SelectCube(0);
	DrawLargeCube(pLargeCube->uwnd);
	InvalidateRect(pLargeCube->HWindow,NULL,FALSE);
	UpdateWindow(pLargeCube->HWindow);
	InvalidateRect(HWindow,NULL,FALSE);
	UpdateWindow(HWindow);
	return;
}

/*-----------------------------------------------------------------*
* Sets up the main display.  Calculates locations of small cubes, *
*   creates child controls, etc.  This is where all of the        *
*   private variables of TMainWindow are initialized.             *
* Entry:  None                                                    *
* Return: None                                                    *
*-----------------------------------------------------------------*/
void TMainWindow::SetupWindow ()
{
	TWindow::SetupWindow();

	FatalError = 0;
	nbr_errors = 0;

	// show the main window maximized.  this simplifies my efforts
	// to construct and maintain order on the display since the
	// window cannot be sized, only minimized and restored to the
	// maximized state.
	//ShowWindow(HWindow,SW_SHOWMAXIMIZED);

	RECT r;
	POINT p;
	const int bw = 110;   // button width
	int bh = 30;    // button height

	// animation stage is zero
	stage = 0;
	bAnimatingSolution = 0;

	// get size of client area of display
	GetClientRect(HWindow,&r);

	// pull 10 pixels away from the edges of the window
	r.left += 10;
	r.right -= 10;
	r.top += 10;
	r.bottom -= 10;

	// nbr of pixels in x & y directions
	int cxs = r.right-r.left;
	int cys = r.bottom-r.top;

	// status line @ bottom of window
	HDC hdc = GetDC(HWindow);
	SIZE textSize;
	GetTextExtentPoint32(hdc,StartStatus,strlen(StartStatus),&textSize);
	bh = textSize.cy + textSize.cy/2;
	rStatus.right = r.right;
	rStatus.bottom = r.bottom;
	rStatus.left = r.left;
	rStatus.top = rStatus.bottom-textSize.cy;
	ReleaseDC(HWindow,hdc);

	//pStatus = new TStatic (this,IDW_STATUS,StartStatus,rStatus.left,
	//	rStatus.top,rStatus.right-rStatus.left,
	//	rStatus.bottom-rStatus.top,150);
	hStatus = CreateWindow("STATIC","Preparing...",WS_VISIBLE | WS_CHILD,
		rStatus.left,
		rStatus.top,
		rStatus.right-rStatus.left,
		rStatus.bottom-rStatus.top,
		HWindow,(HMENU)IDW_STATUS,hInstance,0);

	// buttons across bottom of window
	p.y = rStatus.top - 10 - bh;
	p.x = r.left;
	//new TButton (this,IDW_BTNROTATELEFT,
	//	"Rotate Left",p.x,p.y,bw,bh,FALSE);
	HWND hBtnRL = CreateWindow("BUTTON","Rotate Left",WS_VISIBLE|WS_CHILD,p.x,p.y,bw,bh,HWindow,(HMENU)IDW_BTNROTATELEFT,hInstance,0);
	p.x += bw;
	//new TButton (this,IDW_BTNROTATEDOWN,
	//	"Rotate Down",p.x,p.y,bw,bh,FALSE);
	HWND hBtnRD = CreateWindow("BUTTON","Rotate Down",WS_VISIBLE|WS_CHILD,p.x,p.y,bw,bh,HWindow,(HMENU)IDW_BTNROTATEDOWN,hInstance,0);
	p.x += bw;
	//new TButton (this,IDW_BTNROTATERIGHT,
	//	"Rotate Right",p.x,p.y,bw,bh,FALSE);
	HWND hBtnRR = CreateWindow("BUTTON","Rotate Right",WS_VISIBLE|WS_CHILD,p.x,p.y,bw,bh,HWindow,(HMENU)IDW_BTNROTATERIGHT,hInstance,0);
	p.x += bw;
	//new TButton (this,IDW_BTNROTATEUP,
	//	"Rotate Up",p.x,p.y,bw,bh,FALSE);
	HWND hBtnRU = CreateWindow("BUTTON","Rotate Up",WS_VISIBLE|WS_CHILD,p.x,p.y,bw,bh,HWindow,(HMENU)IDW_BTNROTATEUP,hInstance,0);
	p.x += bw;
	//new TButton (this,IDW_BTNROTATECORNER,
	//	"Rotate Corner",p.x,p.y,bw,bh,FALSE);
	HWND hBtnRC = CreateWindow("BUTTON","Rotate Corner",WS_VISIBLE|WS_CHILD,p.x,p.y,bw,bh,HWindow,(HMENU)IDW_BTNROTATECORNER,hInstance,0);

	// small cubes setup, but not drawing
	scwh = (cxs-140)/NBRSMALLCUBES;
	int i;
	p.y = 10;
	for (i=0; i<NBRSMALLCUBES; i++) {
		sc[i].SetEquation(SmallCubeBaseRectP1,SmallCubeBaseRectP2,
			SmallCubeHeight);
		memmove(&sc[i].offset,&SMALLCUBEOFFSET,sizeof(DPOINT3D));
		sc[i].BorderColor(RGB(0,0,0));
		sc[i].bHiddenFaces = 1;
		sc[i].bColorFaces = 1;
		sc[i].crFace[0] = RGB(0,0,255);
		sc[i].crFace[1] = RGB(0,255,0);
		sc[i].crFace[2] = RGB(0,255,255);
		sc[i].crFace[3] = RGB(255,0,0);
		sc[i].crFace[4] = RGB(255,0,255);
		sc[i].crFace[5] = RGB(255,255,0);
		screct[i].left = r.left + i*(scwh+10+10);
		screct[i].top = r.top+10;
		screct[i].right = screct[i].left+scwh;
		screct[i].bottom = screct[i].top+scwh;
	}
	pSmallCube = new TSmallCubeWindow (this,r.right+10,r.bottom+10,scwh);

	// wire cube used to draw placeholder in large cube
	wire.SetEquation(SmallCubeBaseRectP1,SmallCubeBaseRectP2,
		SmallCubeHeight);
	wire.BorderColor(RGB(0,0,0));
	wire.bHiddenFaces = 1;
	wire.bColorFaces = 0;

	// black cube used to show drop location
	black.SetEquation(SmallCubeBaseRectP1,SmallCubeBaseRectP2,
		SmallCubeHeight);
	black.BorderColor(RGB(0,0,0));
	black.bHiddenFaces = 1;
	black.bColorFaces = 1;
	memset(black.crFace,128,6*sizeof(COLORREF));


	// large cube
	lcwh = min(cys-(10+scwh+10+20+ 20+bh+10+rStatus.bottom-rStatus.top),
		cxs-(10+ 10));
	lc.SetEquation(LargeCubeBaseRectP1,LargeCubeBaseRectP2,LargeCubeHeight);
	memmove(&lc.offset,&LARGECUBEOFFSET,sizeof(DPOINT3D));
	lc.BorderColor(RGB(0,0,0));
	lc.bHiddenFaces = 1;
	lc.bColorFaces = 0;
	rlcDraw.left = (cxs-lcwh)/2;
	rlcDraw.top = 10+scwh+10+(cys-(10+scwh+10+lcwh+bh+10+rStatus.bottom-rStatus.top))/2;
	rlcDraw.right = rlcDraw.left+lcwh;
	rlcDraw.bottom = rlcDraw.top+lcwh;
	pLargeCube = new TDrawRect3D (this,-1,rlcDraw.left,rlcDraw.top,
		lcwh,lcwh);
	pLC2 = new TDrawRect3D (this,-2,r.right+10,r.bottom+10,lcwh,lcwh);

	// none of the cubes in the large cube are occupied
	memset(occupied,LARGECUBE,NBRSMALLCUBES*sizeof(int));

	// mouse handling setup
	bLButtonDown = FALSE;
	bMouseMoved = FALSE;

	// default setup routine (makes all those child controls
	// we just created appear on the display)
	pSmallCube->SetupWindow();
	pLargeCube->SetupWindow();
	pLC2->windowStyle &= (~WS_VISIBLE);
	pLC2->SetupWindow();

	// 3D viewpoint & drawing areas for cubes
	DPOINT3D view;
	memmove(&view,&SMALLCUBEVIEWPOINT,sizeof(DPOINT3D));
	pSmallCube->uwnd.Init(pSmallCube->HWindow,-1,1,-0.5,1,
		2.5,2.6,&view,FALSE,RGB(0,0,0),GetSysColor(COLOR_WINDOW));
	memmove(&view,&LARGECUBEVIEWPOINT,sizeof(DPOINT3D));
	pLargeCube->uwnd.Init(pLargeCube->HWindow,-2,2,-1,2,
		1,4,&view,FALSE,RGB(0,0,0),GetSysColor(COLOR_WINDOW));
	pLC2->uwnd.Init(pLargeCube->HWindow,-2,2,-1,2,
		1,4,&view,FALSE,RGB(0,0,0),GetSysColor(COLOR_WINDOW));

	// draw large cube
	// the small cubes will be taken care of during the
	// WM_PAINT message
	DrawLargeCube(pLargeCube->uwnd);
	InvalidateRect(pLargeCube->HWindow,NULL,FALSE);
	UpdateWindow(pLargeCube->HWindow);

	// select 1st small cube
	ics = 0;

	// open data file for games
	SETTEXT(ThinkingStatus);
	firstgame();
	ColorCubes();
	SETTEXT(StartStatus);

	return;
}

/*-----------------------------------------------------------------*
* TMainWindow constructor.  Assigns the menu in Moshe.RC to the   *
*   main window.                                                  *
* Entry:  ptr to windows object - parent of the main window       *
*         ptr to a string - title of the main window              *
* Return: None                                                    *
*-----------------------------------------------------------------*/
TMainWindow::TMainWindow(PTWindowsObject AParent, LPSTR ATitle, HINSTANCE AnInstance)
: TWindow(AParent,ATitle,AnInstance)
{
	wcex.lpfnWndProc	= TMainWindow::WndProc;
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDM_MAINMENU);
	windowStyle			= WS_OVERLAPPEDWINDOW;
	X					= CW_USEDEFAULT;
	Y					= SW_SHOWMAXIMIZED;
	W					= CW_USEDEFAULT;
	H					= CW_USEDEFAULT;
}

/*-----------------------------------------------------------------*
* TMainWindow destructor.  Destroys created objects.              *
* Entry:  None                                                    *
* Return: None                                                    *
*-----------------------------------------------------------------*/
TMainWindow::~TMainWindow()
{
	//if (pStatus) delete pStatus;
	if (pLargeCube) delete pLargeCube;
	if (pLC2) delete pLC2;
	if (pSmallCube) delete pSmallCube;
	if (bLButtonDown) KillTimer(HWindow,IDW_TIMER);
};


/*-----------------------------------------------------------------*
* Responds to the WM_PAINT event.  Simply redraws the small cubes *
*   and selects the currently selected cube (to draw the          *
*   highlight)                                                    *
* Entry:  HDC of main window (unused)                             *
*         PAINTSTRUCT of main window (unused)                     *
* Return: None                                                    *
*-----------------------------------------------------------------*/
void TMainWindow::Paint (HDC, PAINTSTRUCT  &pps)
{
	if (bLButtonDown || bMouseMoved) return;

	for (int i=0; i<NBRSMALLCUBES; i++) {
		DrawSmallCube(i, &pps);
	}
	//DrawLargeCube(pLargeCube->uwnd);
	//InvalidateRect(pLargeCube->HWindow,NULL,FALSE);
	//UpdateWindow(pLargeCube->HWindow);

	SelectCube(ics);
}

/*-----------------------------------------------------------------*
* Determines whether the specified small cube has been placed in  *
*   the large cube.                                               *
* Entry:  Integer index of a small cube (0 <= i <= 7)             *
* Return: Integer - 0 if the small cube is not in the large cube  *
*                 - non-zero otherwise                            *
*-----------------------------------------------------------------*/
int TMainWindow::IsInLargeCube (int index)
{
	int i;
	for (i=0; i<NBRSMALLCUBES && occupied[i]!=index; i++);
	return (i<NBRSMALLCUBES);
}

/*-----------------------------------------------------------------*
* Draws the large cube on the specified 3D user window.           *
* Uses the Painter's Algorithm to draw the small cubes inside the *
*   large cube.                                                   *
* Entry:  3D user window to draw on                               *
* Return: None                                                    *
*-----------------------------------------------------------------*/
void TMainWindow::DrawLargeCube (UserWindow3D &u)
{
	// erase the window
	u.Erase();

	// update the point structures for the cube
	lc.UpdatePoints(u);

	// sort the 8 points of the cube in the order in which they
	// are viewed in the window
	int order[NBRSMALLCUBES];
	SortLargeCube(u,order);

	// draw the small cubes (or wire cube) in the reverse order
	// from which they are viewed (this is the also known as
	// the Painter's Algorithm)
	for (int i=NBRSMALLCUBES-1; i>=0; i--) {
		if (occupied[order[i]]<NBRSMALLCUBES) {
			sc[occupied[order[i]]].offset.x =
				lc.offset.x + lc.po[order[i]].x/2.0;
			sc[occupied[order[i]]].offset.y =
				lc.offset.y + lc.po[order[i]].y/2.0;
			sc[occupied[order[i]]].offset.z =
				lc.offset.z + lc.po[order[i]].z/2.0;

			// draw small cube combined with the current background
			// in the user window
			sc[occupied[order[i]]].Draw(u,TRUE);
		}
		else {
			wire.offset.x = lc.offset.x + lc.po[order[i]].x/2.0;
			wire.offset.y = lc.offset.y + lc.po[order[i]].y/2.0;
			wire.offset.z = lc.offset.z + lc.po[order[i]].z/2.0;
			wire.Draw(u);
		}
	}
	return;
}

/*-----------------------------------------------------------------*
* Draws the small cube on the offscreen window, then copies it to *
*   the pre-calculated position on the main window.               *
* Entry:  Integer - index of the cube to be drawn (0 <= i <= 7)   *
* Return: None                                                    *
*-----------------------------------------------------------------*/
void TMainWindow::DrawSmallCube (int i, PAINTSTRUCT *pps)
{
	// don't draw if it is in the large cube
	if (IsInLargeCube(i)) return;

	// erase window
	pSmallCube->uwnd.Erase();

	// draw the small cube on the window (offscreen)
	sc[i].Draw(pSmallCube->uwnd);

	// copy the small cube onto the main window
	HDC dest,src;
	if (pps != NULL) dest = pps->hdc;
	else dest = GetDC(HWindow);
	src = pSmallCube->uwnd.MemScreenDC();
	BOOL bbRet = BitBlt(dest,screct[i].left,screct[i].top,scwh,scwh, src,0,0, SRCCOPY);
	if (pps == NULL) ReleaseDC(HWindow,dest);
	DeleteDC(src);

	return;
}

/*-----------------------------------------------------------------*
* Selects/Deselects the specified cube.  Updates the ics private  *
*   TMainWindow variable and draws a black rectangle around the   *
*   specified cube.                                               *
* Entry:  Integer - index of cube to select/deselect              *
*         Integer (optional) - TRUE = deselect, FALSE = select    *
*           default is FALSE                                      *
* Return: None                                                    *
*-----------------------------------------------------------------*/
void TMainWindow::SelectCube (int index,int deselect)
{
	// don't select if small cube being selected is in
	// the large cube
	if (index<LARGECUBE && IsInLargeCube(index)) return;

	// create the pen to draw with (either black or the
	// window background color depending on whether we
	// are selecting or deselecting the cube)
	HPEN hpen;
	if (deselect) hpen = CreatePen(PS_INSIDEFRAME,5,GetSysColor(COLOR_WINDOW));
	else hpen = CreatePen(PS_INSIDEFRAME,5,RGB(0,0,0));
	HDC hdc = GetDC(HWindow);
	SelectObject(hdc,hpen);
	SelectObject(hdc,GetStockObject(NULL_BRUSH));

	// update the internal data structure
	ics = index;

	// draw the rectangle with the current pen
	if (ics<LARGECUBE) {
		Rectangle(hdc,screct[ics].left-10,screct[ics].top-10,
			screct[ics].right+10,screct[ics].bottom+10);
	}
	else {
		Rectangle(hdc,rlcDraw.left-5,rlcDraw.top-5,
			rlcDraw.right+5,rlcDraw.bottom+5);
	}

	// cleanup
	ReleaseDC(HWindow,hdc);
	DeleteObject(hpen);
	return;
}

/*-----------------------------------------------------------------*
* Checks to see if the puzzle has been solved.                    *
* Entry:  None                                                    *
* Return: Integer - 0 if not solved                               *
*                 - non-zero otherwise                            *
*-----------------------------------------------------------------*/
int TMainWindow::IsSolution (void)
{
	int i,j,is,ip,ips1,ips2;
	DPOINT3D p,p0;

	// delta's from the center of the large cube to the surface
	// points on the large cube's faces
	static const DPOINT3D surfacedelta[NBRFACES][5] = {
		{{-1,0,0},{-1,-1,-1},{-1,1,1},{-1,-1,1},{-1,1,-1}},
		{{0,1,0},{1,1,1},{-1,1,-1},{-1,1,1},{1,1,-1}},
		{{1,0,0},{1,1,1},{1,-1,-1},{1,-1,1},{1,1,-1}},
		{{0,-1,0},{-1,-1,-1},{1,-1,1},{-1,-1,1},{1,-1,-1}},
		{{0,0,1},{1,1,1},{-1,-1,1},{1,-1,1},{-1,1,1}},
		{{0,0,-1},{-1,-1,-1},{1,1,-1},{1,-1,-1},{-1,1,-1}}
	};

	// are all small cubes in large cube?
	for (i=0; i<NBRSMALLCUBES; i++) if (occupied[i]>=NBRSMALLCUBES) return FALSE;

	// find the colors for all faces of the large cube
	for (i=0; i<NBRFACES; i++) {
		p.x = lc.offset.x + surfacedelta[i][1].x;
		p.y = lc.offset.y + surfacedelta[i][1].y;
		p.z = lc.offset.z + surfacedelta[i][1].z;
		ip = lc.IndexFromPoint(p);
		is = occupied[ip];
		ips1 = sc[is].IndexFromPoint(p);
		p.x = lc.offset.x + surfacedelta[i][0].x;
		p.y = lc.offset.y + surfacedelta[i][0].y;
		p.z = lc.offset.z + surfacedelta[i][0].z;
		ips2 = sc[is].IndexFromPoint(p);
		lc.crFace[i] = sc[is].crFace[sc[is].FaceFromPointIndices(ips1,ips2)];
	}

	// are any colors on the faces of the large cube repeated
	for (i=0; i<NBRFACES; i++) {
		for (j=i+1; j<NBRFACES; j++) {
			if (lc.crFace[i]==lc.crFace[j]) return FALSE;
		}
	}

	// make sure all the small cubes match the colors found above
	for (i=0; i<NBRFACES; i++) {
		p0.x = lc.offset.x + surfacedelta[i][0].x;
		p0.y = lc.offset.y + surfacedelta[i][0].y;
		p0.z = lc.offset.z + surfacedelta[i][0].z;
		for (j=2; j<=4; j++) {
			p.x = lc.offset.x + surfacedelta[i][j].x;
			p.y = lc.offset.y + surfacedelta[i][j].y;
			p.z = lc.offset.z + surfacedelta[i][j].z;
			ip = lc.IndexFromPoint(p);
			is = occupied[ip];
			ips1 = sc[is].IndexFromPoint(p);
			ips2 = sc[is].IndexFromPoint(p0);
			if (lc.crFace[i] != sc[is].crFace[sc[is].FaceFromPointIndices(ips1,ips2)])
				return FALSE;
		}
	}

	return TRUE;
}		

/*-----------------------------------------------------------------*
* Sorts the 8 corner points of the large cube in the order in     *
*   which they are viewed from the viewpoint.                     *
* Entry:  3D user window                                          *
*         Integer array to store the point indices in the sorted  *
*           order                                                 *
* Return: None                                                    *
*-----------------------------------------------------------------*/
void TMainWindow::SortLargeCube (UserWindow3D &u, int *order)
{
	int i,j;
	DPOINT3D v;
	//int torder;
	double mind[NBRCUBEPOINTS], dt;
	double dx,dy,dz;

	// get the point of perspective
	u.GetViewpoint(&v);

	// distance from viewpoint to outside corner 0 of large cube
	dx = lc.pr[0].x - v.x;
	dy = lc.pr[0].y - v.y;
	dz = lc.pr[0].z - v.z;
	mind[0] = dx*dx + dy*dy + dz*dz;
	order[0] = 0;

	for (i=1; i<NBRCUBEPOINTS; i++) {
		// distance from viewpoint to outside corner i of large cube
		dx = lc.pr[i].x - v.x;
		dy = lc.pr[i].y - v.y;
		dz = lc.pr[i].z - v.z;
		mind[i] = dt = dx*dx + dy*dy + dz*dz;
		order[i] = i;

		// insert that point into sorted order, closest first
		for (j=0; j<i; j++) {
			if (dt<mind[j]) {
				memmove(mind+j+1,mind+j,(i-j)*sizeof(double));
				memmove(order+j+1,order+j,(i-j)*sizeof(int));
				mind[j] = dt;
				order[j] = i;
				break;
			}
		}
	}
	return;
}

/*-----------------------------------------------------------------*
* Starts a new game.                                              *
* Entry:  Event message                                           *
* Return: None                                                    *
*-----------------------------------------------------------------*/
void TMainWindow::TMainWindowIDM_GAMENEW (RTMessage Msg)
{
	if (bAnimatingSolution) return;

	// thinking message
	SETTEXT(ThinkingStatus);

	int i;

	// no small cubes are in the large cube
	for (i=0; i<NBRSMALLCUBES; i++) occupied[i] = NBRSMALLCUBES;

	// reset small & large cubes to original orientation
	// & location
	for (i=0; i<NBRSMALLCUBES; i++) {
		sc[i].SetEquation(SmallCubeBaseRectP1,SmallCubeBaseRectP2,
			SmallCubeHeight);
		memmove(&sc[i].offset,&SMALLCUBEOFFSET,sizeof(DPOINT3D));
	}
	wire.SetEquation(SmallCubeBaseRectP1,SmallCubeBaseRectP2,
		SmallCubeHeight);
	lc.SetEquation(LargeCubeBaseRectP1,LargeCubeBaseRectP2,
		LargeCubeHeight);
	memmove(&lc.offset,&LARGECUBEOFFSET,sizeof(DPOINT3D));

	// load next puzzle
	nextgame();

	// color small cubes
	ColorCubes();

	// repaint everything
	InvalidateRect(HWindow,NULL,FALSE);
	DrawLargeCube(pLargeCube->uwnd);
	InvalidateRect(pLargeCube->HWindow,NULL,FALSE);
	UpdateWindow(pLargeCube->HWindow);
	SETTEXT(StartStatus);
}

/*-----------------------------------------------------------------*
* Reports non-fatal errors in the form of a messagebox with the   *
* stop sign.                                                      *
* Entry: Pointer to the name of the function in which the error   *
*           occurred.                                             *
*        Integer identifier (to find where the error occurred     *
*           within the function).                                 *
* Return: None                                                    *
*-----------------------------------------------------------------*/
void TMainWindow::ReportError (const char *szFunctionName, const int id)
{
	char text[80];
	if (nbr_errors > 5) {
		FatalError = 1;
		MessageBoxA(NULL,"Too many errors!",AppName,MB_OK|MB_ICONSTOP);
		CloseWindow();
	}
	else {
		++nbr_errors;
		wsprintfA(text,"Non-Fatal Error Detected in %s(%d)",szFunctionName,id);
		if (!FatalError) MessageBoxA(HWindow,text,AppName,MB_OK|MB_ICONSTOP);
	}
	return;
}
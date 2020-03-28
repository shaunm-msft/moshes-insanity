#define _USE_MATH_DEFINES
#include <math.h>
#include <string.h>
#include "moshe.h"
#include "mosh-con.h"

/*-----------------------------------------------------------------*
* Handles the WM_LBUTTONUP event.  If a small cube has been drug  *
*   around the screen, drop it in the appropriate location.       *
* Entry:  Event message (unused)                                  *
* Return: None                                                    *
*-----------------------------------------------------------------*/
void TMainWindow::WMLButtonUp (RTMessage)
{
	// no need to capture mouse messages outside the window
	ReleaseCapture();

	// done dragging
	if (bMouseMoved && ics<LARGECUBE) DropCube();

	// left button is up
	bLButtonDown = FALSE;

	return;
}

/*-----------------------------------------------------------------*
* Handles the WM_MOUSEMOVE event.  Does all the work for dragging *
*   the small cubes around the screen and calculate where it      *
*   should be dropped when the dragging is done                   *
* Entry:  Event message                                           *
* Return: None                                                    *
*-----------------------------------------------------------------*/
void TMainWindow::WMMouseMove (RTMessage Msg)
{
	// can't drag the large cube
	if (ics>=LARGECUBE) return;

	// don't do anything if the left button wasn't pressed
	if (bLButtonDown) MoveCube(Msg);

	return;
}

#define _USE_MATH_DEFINES
#include <math.h>
#include <string.h>
#include "moshe.h"

/*-----------------------------------------------------------------*
* Handles the WM_LBUTTONDOWN event.  Changes the currently        *
*   selected cube.  If the large cube is already selected, then   *
*   attempt to locate the small cube be selected out of the large *
*   cube.                                                         *
* Entry:  Event message                                           *
* Return: None                                                    *
*-----------------------------------------------------------------*/
void TMainWindow::WMLButtonDown (RTMessage Msg)
{
	// nothing to do if the left button is already down
	// I don't know how this can occur, but better safe
	// than GPF...
	if (bLButtonDown || bAnimatingSolution) return;

	// cursor hasn't moved yet!
	bMouseMoved = FALSE;

	// store the current and anchor mouse positions
	cmpos.x = mpos.x = LOWORD(Msg.lParam);
	cmpos.y = mpos.y = HIWORD(Msg.lParam);

	// find the small cube being clicked on
	int i;
	for (i=0; i<NBRSMALLCUBES; i++) {
		if (PtInRect(&screct[i],mpos)) break;
	}
	if (i<NBRSMALLCUBES && IsInLargeCube(i)) {
		MessageBeep(MB_ICONQUESTION);
		return;
	}

	// attempt to select a small cube within the large cube
	if (i==LARGECUBE && ics==LARGECUBE) {

		// convert click point to large cube window coordinates
		POINT mp;
		mp.x = mpos.x - rlcDraw.left;
		mp.y = mpos.y - rlcDraw.top;

		// check for a small cube existing at the click point
		// if not, nothing else to do
		HDC hdcLargeCube = pLargeCube->uwnd.MemScreenDC();
		COLORREF crPt = GetPixel(hdcLargeCube,mp.x,mp.y);
		DeleteDC(hdcLargeCube);
		if ( crPt == GetSysColor(COLOR_WINDOW)) return;

		// update points on large cube
		lc.UpdatePoints(pLargeCube->uwnd);

		// find the cube clicked on
		// first sort the points of the large cube
		// then go through each small cube in the large cube from
		// closest to furthest and check each face to see if it
		// contains the click point 
		int order[NBRSMALLCUBES];
		int c,f,p;
		RECT r;
		int selectedcube = NBRSMALLCUBES;
		SortLargeCube(pLargeCube->uwnd,order);
		for (i=0; i<NBRSMALLCUBES && selectedcube==NBRSMALLCUBES; i++) {
			p = order[i];
			if (occupied[p]>=NBRSMALLCUBES) continue;
			c = occupied[p];
			for (f=0; f<NBRFACES; f++) {
				if (sc[c].bHideFace[f]) continue;
				r.left =   min(sc[c].ps[sc[c].fp[f][0]].x,
					sc[c].ps[sc[c].fp[f][2]].x);
				r.right =  max(sc[c].ps[sc[c].fp[f][0]].x,
					sc[c].ps[sc[c].fp[f][2]].x);
				r.top =    min(sc[c].ps[sc[c].fp[f][0]].y,
					sc[c].ps[sc[c].fp[f][2]].y);
				r.bottom = max(sc[c].ps[sc[c].fp[f][0]].y,
					sc[c].ps[sc[c].fp[f][2]].y);
				if (PtInRect(&r,mp)) {
					selectedcube = c;
					break;
				}
			}
		} 
		// must have click on a line, not a cube
		if (selectedcube==NBRSMALLCUBES) return;

		// deselect large cube
		SelectCube(ics,TRUE);

		// select small cube
		ics = selectedcube;

		// position in large cube is now unoccupied
		occupied[p] = NBRSMALLCUBES;

		// drag selected small cube
		// we have to do a little prep work here so that the
		// mousemove event can take over
		memmove(&sc[ics].offset,&SMALLCUBEOFFSET,sizeof(DPOINT3D));
		if (lc.pr[p].y < lc.offset.y) {
			if (lc.pr[p].x < lc.offset.x)
				;
			else if (lc.pr[p].x > lc.offset.x)
				sc[ics].Rotate(0,0,-M_PI_2);
		}
		else if (lc.pr[p].y > lc.offset.y) {
			if (lc.pr[p].x < lc.offset.x)
				sc[ics].Rotate(0,0,M_PI_2);
			else if (lc.pr[p].x > lc.offset.x)
				sc[ics].Rotate(0,0,M_PI);
		}
		DrawLargeCube(pLargeCube->uwnd);
		InvalidateRect(pLargeCube->HWindow,NULL,FALSE);
		UpdateWindow(pLargeCube->HWindow);
		mpos.x = screct[ics].left + scwh/2;
		mpos.y = screct[ics].top + scwh/2;
		MoveCube(Msg);
	}
	else {

		// don't undo what we are going to redo
		// ie. new selection = old selection
		if (i!=ics) SelectCube(ics,TRUE);

		// don't avoid doing this when old sel = new sel
		// in case the highlight has been destroyed
		SelectCube(i);
	}

	// left button is down
	bLButtonDown = TRUE;

	// capture all mouse messages - inside & outside the window
	SetCapture(HWindow);

	return;
}


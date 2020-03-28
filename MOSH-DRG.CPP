#define _USE_MATH_DEFINES
#include <math.h>
#include <string.h>
#include "moshe.h"
#include "mosh-con.h"

extern char WinStatus[];
extern char szMoveCube[];
extern char szDropCube[];

/*-----------------------------------------------------------------*
* Called when a small cube is being dragged (hopefully every      *
*   mouse movement).  The first time the cube is moved, a bunch   *
*   of resources must be allocated.  After that, the scmall cube  *
*   just has to be drawn with a transparent background.           *
* Entry:  Reference to a TMessage                                 *
* Return: None                                                    *
*-----------------------------------------------------------------*/
void TMainWindow::MoveCube (RTMessage Msg)
{
	HGDIOBJ hr;
	BOOL br;
	RECT r;

	// if the mouse hasn't moved yet, then set up offscreen
	// bitmaps, DCs, etc.
	if (!bMouseMoved) {

		// mouse has moved
		bMouseMoved = TRUE;

		// deselect cube
		SelectCube(ics,TRUE);

		// drop small cube in original location
		masterindex = LARGECUBE;

		// correct the orientation of the black cube that marks
		// where the small cube will be dropped in the large
		// cube
		memmove(black.po,wire.po,NBRCUBEPOINTS*sizeof(DPOINT3D));

		// secondary display (copy of screen)
		GetClientRect(HWindow,&r);
		HDC temp = GetDC(HWindow);
		if (!temp) ReportError(szMoveCube,1);
		hdcMain = CreateCompatibleDC(temp);
		if (!hdcMain) ReportError(szMoveCube,2);
		hbmpMain = CreateCompatibleBitmap(temp,r.right-r.left,
			r.bottom-r.top);
		if (!hbmpMain) ReportError(szMoveCube,3);
		hr = SelectObject(hdcMain,hbmpMain);
		if (!hr) ReportError(szMoveCube,4);
		br = BitBlt(hdcMain,0,0,r.right-r.left,r.bottom-r.top,
			temp,0,0, SRCCOPY);
		if (!br) ReportError(szMoveCube,5);
		br = ReleaseDC(HWindow,temp);
		if (!br) ReportError(szMoveCube,6);


		// draw large cube offscreen
		DrawLargeCube(pLC2->uwnd);

		// background of small colored cube
		hbmpBackground = CreateCompatibleBitmap(hdcMain,scwh,scwh);
		if (!hbmpBackground) ReportError(szMoveCube,7);
		hdcBackground = CreateCompatibleDC(hdcMain);
		if (!hdcBackground) ReportError(szMoveCube,8);
		hr = SelectObject(hdcBackground,hbmpBackground);
		if (!hr) ReportError(szMoveCube,9);
		hr = SelectObject(hdcBackground,CreateSolidBrush(GetSysColor(COLOR_WINDOW)));
		if (!hr) ReportError(szMoveCube,10);
		br = PatBlt(hdcBackground,0,0,scwh,scwh,PATCOPY);
		if (!br) ReportError(szMoveCube,11);
		// hide the large cube (it will be drawn during dragging
		// by the offscreen large cube)
		pLargeCube->Show(SW_HIDE);
	}

	// Replace small cube background on secondary display
	br = BitBlt(hdcMain,screct[ics].left+(cmpos.x-mpos.x),
		screct[ics].top+(cmpos.y-mpos.y),scwh,scwh,
		hdcBackground,0,0, SRCCOPY);
	if (!br) ReportError(szMoveCube,12);

	// store old mouse location
	POINT oldpos;
	oldpos.x = cmpos.x;
	oldpos.y = cmpos.y;
	// store new mouse location 
	cmpos.x = LOWORD(Msg.lParam);
	cmpos.y = HIWORD(Msg.lParam);

	// find closest open drop spot to new mouse location
	// by finding the shortest distance between the bottom,
	// right corner of the cube being dragged and one of
	// the 8 corner points on the large cube
	POINT p;
	p.x = screct[ics].right+(cmpos.x-mpos.x);
	p.y = screct[ics].bottom+(cmpos.y-mpos.y);
	if (PtInRect(&rlcDraw,p)) {
		long mindist = (long)p.x*(long)p.x + (long)p.y*(long)p.y;
		int index = LARGECUBE;
		long dist;
		long dx,dy;
		for (int i=0; i<NBRCUBEPOINTS; i++) {
			dx = rlcDraw.left+lc.ps[i].x-p.x;
			dy = rlcDraw.top+lc.ps[i].y-p.y;
			dist = dx*dx + dy*dy;
			if (dist < mindist) {
				mindist = dist;
				index = i;
			}
		}
		// if the drop point is different, draw the marker
		// cube in its new location
		if (index<LARGECUBE && index!=masterindex && occupied[index]>7) {
			masterindex = index;
			black.offset.x = lc.offset.x + lc.po[index].x/2.0;
			black.offset.y = lc.offset.y + lc.po[index].y/2.0;
			black.offset.z = lc.offset.z + lc.po[index].z/2.0;
			// Draw large cube offscreen
			DrawLargeCube(pLC2->uwnd);
			// Draw black cube on large cube offscreen
			black.Draw(pLC2->uwnd,TRUE);
		}
	}
	else if (masterindex<LARGECUBE) {
		masterindex = LARGECUBE;
		DrawLargeCube(pLC2->uwnd);
	}

	// replace large cube on secondary display
	HDC hdcLargeCube = pLC2->uwnd.MemScreenDC();
	if (!hdcLargeCube) ReportError(szMoveCube,13);
	br = BitBlt(hdcMain,rlcDraw.left,rlcDraw.top,lcwh,lcwh,
		hdcLargeCube,0,0,SRCCOPY);
	if (!br) ReportError(szMoveCube,14);
	br = DeleteDC(hdcLargeCube);
	if (!br) ReportError(szMoveCube,15);

	// get new background for small cube
	br = BitBlt(hdcBackground,0,0,scwh,scwh,
		hdcMain,screct[ics].left+(cmpos.x-mpos.x),
		screct[ics].top+(cmpos.y-mpos.y),SRCCOPY);
	if (!br) ReportError(szMoveCube,16);

	// replace small cube on secondary display
	sc[ics].Draw(pSmallCube->uwnd,FALSE,hdcBackground,scwh);
	HDC hdcCube = pSmallCube->uwnd.MemScreenDC();
	if (!hdcCube) ReportError(szMoveCube,17);
	br = BitBlt(hdcMain,screct[ics].left+(cmpos.x-mpos.x),
		screct[ics].top+(cmpos.y-mpos.y),scwh,scwh, 
		hdcCube,0,0, SRCCOPY);
	if (!br) ReportError(szMoveCube,18);
	br = DeleteDC(hdcCube);
	if (!br) ReportError(szMoveCube,19);

	// replace display with secondary display
	HDC hdcDisplay = GetDC(HWindow);
	if (!hdcDisplay) ReportError(szMoveCube,20);
	r.left = min(screct[ics].left+(oldpos.x-mpos.x),rlcDraw.left);
	r.left = min(screct[ics].left+(cmpos.x-mpos.x),r.left);
	r.right = max(screct[ics].right+(cmpos.x-mpos.x),rlcDraw.right);
	r.right = max(screct[ics].right+(oldpos.x-mpos.x),r.right);
	r.top = min(screct[ics].top+(oldpos.y-mpos.y),rlcDraw.top);
	r.top = min(screct[ics].top+(cmpos.y-mpos.y),r.top);
	r.bottom = max(screct[ics].bottom+(cmpos.y-mpos.y),rlcDraw.bottom);
	r.bottom = max(screct[ics].bottom+(oldpos.y-mpos.y),r.bottom);
	br = BitBlt(hdcDisplay,r.left,r.top,r.right-r.left,r.bottom-r.top,
		hdcMain,r.left,r.top, SRCCOPY);
	if (!br) ReportError(szMoveCube,21);
	br = ReleaseDC(HWindow,hdcDisplay);
	if (!br) ReportError(szMoveCube,22);

	return;
}

/*-----------------------------------------------------------------*
* Called when a small cube is being dragged and is finally        *
*   dropped.  Cleans up resources allocated for the drag and      *
*   either draws the small cube in its position at the top of the *
*   window or within the large cube.                              *
* Entry:  none                                                    *
* Return: None                                                    *
*-----------------------------------------------------------------*/
void TMainWindow::DropCube (void)
{
	BOOL br;

	// Replace small cube background on secondary display
	HDC temp = GetDC(HWindow);
	if (!temp) ReportError(szDropCube,1);
	br = BitBlt(temp,screct[ics].left+(cmpos.x-mpos.x),
		screct[ics].top+(cmpos.y-mpos.y),scwh,scwh,
		hdcBackground,0,0, SRCCOPY);
	if (!br) ReportError(szDropCube,2);
	br = ReleaseDC(HWindow,temp);
	if (!br) ReportError(szDropCube,3);

	// cleanup DCs & BMPs used in dragging
	br = DeleteDC(hdcMain);
	if (!br) ReportError(szDropCube,4);
	br = DeleteObject(hbmpMain);
	if (!br) ReportError(szDropCube,5);
	DeleteObject(SelectObject(hdcBackground,(HGDIOBJ)NULL_BRUSH));
	br = DeleteDC(hdcBackground);
	if (!br) ReportError(szDropCube,6);
	br = DeleteObject(hbmpBackground);
	if (!br) ReportError(szDropCube,7);

	// drop the small cube
	// if the place holder is in the large cube, drop it in
	// the large cube, otherwise, put it back in its original
	// position
	if (masterindex<LARGECUBE) {

		// translate small cube to its position in large cube
		sc[ics].offset.x = lc.offset.x + lc.po[masterindex].x/2.0;
		sc[ics].offset.y = lc.offset.y + lc.po[masterindex].y/2.0;
		sc[ics].offset.z = lc.offset.z + lc.po[masterindex].z/2.0;

		// rotate small cube into position
		// the small cube is already square with the large cube
		// so only the offset needs to be changed for it to fit
		// inside the large cube.  the edge of the small cube
		// closest the viewpoint still needs to be rotated to
		// the outside corner edge of the large cube, which is
		// only a rotation around the z axis.  So, since we
		// know the orientation of the small cube already, we
		// only need to find which outside corner edge on the
		// large cube we need to rotate to.  this can be determined
		// from the relationship the outside points of the large
		// cube have with the center of the large cube.
		if (lc.pr[masterindex].y < lc.offset.y) {
			if (lc.pr[masterindex].x < lc.offset.x)
				;
			else if (lc.pr[masterindex].x > lc.offset.x)
				sc[ics].Rotate(0,0,M_PI_2);
		}
		else if (lc.pr[masterindex].y > lc.offset.y) {
			if (lc.pr[masterindex].x < lc.offset.x)
				sc[ics].Rotate(0,0,-M_PI_2);
			else if (lc.pr[masterindex].x > lc.offset.x)
				sc[ics].Rotate(0,0,M_PI);
		}

		// mark position as occupied by small cube
		occupied[masterindex] = ics;

		// select large cube
		SelectCube(LARGECUBE);

		// check to see if the puzzle has been solved
		if (IsSolution()) SETTEXT(WinStatus);

	}
	else DrawSmallCube(ics);

		// draw large cube
	DrawLargeCube(pLargeCube->uwnd);
	InvalidateRect(pLargeCube->HWindow,NULL,FALSE);
	UpdateWindow(pLargeCube->HWindow);

	// unhide large cube (hidden at 1st mouse movement)
	pLargeCube->Show(SW_SHOWNORMAL);

	bMouseMoved = FALSE;

	return;
}

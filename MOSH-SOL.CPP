#define _USE_MATH_DEFINES
#include <math.h>
#include <string.h>
#include "moshe.h"
#include "mosh-con.h"

extern char StartStatus[];
extern char AnimateSolutionStatus[];
extern char TimerErrorMsg[];
extern char AppName[];

extern char cube[8][6];
extern char corner[8][8][3];    // Left,Right,Top
extern int bottom[4][3];        // cube,corner,rotation
extern int top[4][3];
void firstgame (void);
void nextgame (void);

/*-----------------------------------------------------------------*
* Colors the small cubes with the colors listed in the external   *
*   color array (located in solution.cpp.                         *
* Entry:  None                                                    *
* Return: None                                                    *
*-----------------------------------------------------------------*/
void TMainWindow::ColorCubes (void)
{
	int c,f;

	for (c=0; c<NBRSMALLCUBES; c++) {
		for (f=0; f<NBRFACES; f++) {
			switch (cube[c][f]) {
				case 'R': sc[c].crFace[f] = RGB(255,0,0); break;
				case 'G': sc[c].crFace[f] = RGB(0,255,0); break;
				case 'B': sc[c].crFace[f] = RGB(0,0,255); break;
				case 'C': sc[c].crFace[f] = RGB(0,255,255); break;
				case 'M': sc[c].crFace[f] = RGB(255,0,255); break;
				case 'Y': sc[c].crFace[f] = RGB(255,255,0); break;
			}
		}
	}
	return;
}

/*-----------------------------------------------------------------*
* Performs the requested animation - either of the rotation of a  *
*   single cube or of the solution to the puzzle - given by the   *
*   value of bAnimatingSolution.                                  *
* Assumes:  xrad, yrad, zrad,stage,nbrstages,bAnimatingSolution,  *
*           steplist                                              *
*   (all TMainWindow private variables) have been properly set.   *
* Entry:  Event message (unused).                                 *
* Return: None                                                    *
*-----------------------------------------------------------------*/
void TMainWindow::TMainWindowIDW_TIMER (RTMessage)
{
	if (FatalError) return;

	static StepLanguage a, *t;

	// done with animation?
	if (stage>nbrstages) {
		stage = 0;
		if (!bAnimatingSolution || !steplist.Size()) {
			KillTimer(HWindow,IDW_TIMER);
			bAnimatingSolution = FALSE;
			SETTEXT(StartStatus);
			return;
		}
		a = *(t=steplist.RemoveHead());
		delete t;
	}
	// maybe user is rotating a cube
	if (!bAnimatingSolution) {
		AnimatedRotateCube();
		return;
	}

	// we are now animating the solution to the puzzle
	switch (a.op) {
		case StepLanguage::cubeselect:
			SelectCube(ics,TRUE);
			SelectCube(a.arg.cube);
			stage = nbrstages+1;
			return;
		case StepLanguage::cuberotate:
			if (!stage) {
				xrad = a.arg.radians[0];
				yrad = a.arg.radians[1];
				zrad = a.arg.radians[2];
			}
			AnimatedRotateCube();
			return;
		case StepLanguage::cubemovetopoint:
			AnimatedMoveCube(a.arg.point);
			return;
	}
	stage = nbrstages+1;
	return;
}

/*-----------------------------------------------------------------*
* Animates a solution to the current game.                        *
* Entry:  Event message                                           *
* Return: None                                                    *
*-----------------------------------------------------------------*/
void TMainWindow::TMainWindowIDM_GAMESOLVE (RTMessage Msg)
{
	if (bAnimatingSolution) return;

	StepLanguage a;
	int i,j;
	int currentrotation;

	// reset small & large cubes to original orientation
	// & location
	for (i=0; i<NBRSMALLCUBES; i++) {
		sc[i].SetEquation(SmallCubeBaseRectP1,SmallCubeBaseRectP2,
			SmallCubeHeight);
		memmove(&sc[i].offset,&SMALLCUBEOFFSET,sizeof(DPOINT3D));
		occupied[i] = NBRSMALLCUBES;
	}
	wire.SetEquation(SmallCubeBaseRectP1,SmallCubeBaseRectP2,
		SmallCubeHeight);
	lc.SetEquation(LargeCubeBaseRectP1,LargeCubeBaseRectP2,
		LargeCubeHeight);
	memmove(&lc.offset,&LARGECUBEOFFSET,sizeof(DPOINT3D));
	InvalidateRect(HWindow,NULL,FALSE);
	UpdateWindow(HWindow);
	DrawLargeCube(pLargeCube->uwnd);
	InvalidateRect(pLargeCube->HWindow,NULL,FALSE);
	UpdateWindow(pLargeCube->HWindow);

	// bottom layer of small cubes (in large cube)
	for (i=0; i<4; i++) {

		// select which cube we're working with
		a.op = StepLanguage::cubeselect;
		a.arg.cube = bottom[i][0];
		steplist.Append(a);

		// rotate the corner into position
		a.op = StepLanguage::cuberotate;
		if (bottom[i][1]<4) {
			a.arg.radians[0] = 0;
			a.arg.radians[1] = 0;
			a.arg.radians[2] = ((bottom[i][1]+3)%4)*M_PI_2;
			steplist.Append(a);
			currentrotation = 3;
		}
		else {
			a.arg.radians[0] = 0;
			a.arg.radians[1] = 0;
			a.arg.radians[2] = ((bottom[i][1]-1)%4)*M_PI_2;
			steplist.Append(a);
			a.arg.radians[0] = 0;
			a.arg.radians[1] = M_PI_2;
			a.arg.radians[2] = 0;
			steplist.Append(a);
			currentrotation = 2;
		}
		// give the corner the correct rotation
		// (each corner has 3 possible rotations)
		for (j=0; j<4 && currentrotation!=bottom[i][2]; j++) {
			a.arg.radians[0] = -M_PI_2;
			a.arg.radians[1] = 0;
			a.arg.radians[2] = M_PI_2;
			steplist.Append(a);
			--currentrotation;
			if (!currentrotation) currentrotation=3;
		}
		// turn cube upside down to be placed in large cube
		a.arg.radians[0] = 0;
		a.arg.radians[1] = M_PI;
		a.arg.radians[2] = -M_PI_2;
		steplist.Append(a);

		// drag & drop small cube in large cube
		a.op = StepLanguage::cubemovetopoint;
		switch(i) {
				case 0: a.arg.point = 1; break;
				case 1: a.arg.point = 2; break;
				case 2: a.arg.point = 0; break;
				case 3: a.arg.point = 3; break;
		}
		steplist.Append(a);
	} // end bottom layer of cubes

	// top layer of small cubes (in large cube)
	for (i=0; i<4; i++) {

		// select which cube we're working with
		a.op = StepLanguage::cubeselect;
		a.arg.cube = top[i][0];
		steplist.Append(a);

		// rotate the corner into position
		a.op = StepLanguage::cuberotate;
		if (top[i][1]<4) {
			a.arg.radians[0] = 0;
			a.arg.radians[1] = 0;
			a.arg.radians[2] = ((top[i][1]+3)%4)*M_PI_2;
			steplist.Append(a);
			currentrotation = 3;
		}
		else {
			a.arg.radians[0] = 0;
			a.arg.radians[1] = 0;
			a.arg.radians[2] = ((top[i][1]-1)%4)*M_PI_2;
			steplist.Append(a);
			a.arg.radians[0] = 0;
			a.arg.radians[1] = M_PI_2;
			a.arg.radians[2] = 0;
			steplist.Append(a);
			currentrotation = 2;
		}
		// give the corner the correct rotation
		// (each corner has 3 possible rotations)
		for (j=0; j<4 && currentrotation!=top[i][2]; j++) {
			a.arg.radians[0] = -M_PI_2;
			a.arg.radians[1] = 0;
			a.arg.radians[2] = M_PI_2;
			steplist.Append(a);
			--currentrotation;
			if (!currentrotation) currentrotation=3;
		}

		// drag & drop small cube in large cube
		a.op = StepLanguage::cubemovetopoint;
		switch(i) {
				case 0: a.arg.point = 4; break;
				case 1: a.arg.point = 6; break;
				case 2: a.arg.point = 5; break;
				case 3: a.arg.point = 7; break;
		}
		steplist.Append(a);
	} // end top layer of cubes

	// rotate the large cube 360 around z axis & 90 around y axis
	a.op = StepLanguage::cubeselect;
	a.arg.cube = LARGECUBE;
	steplist.Append(a);
	a.op = StepLanguage::cuberotate;
	a.arg.radians[0] = 0;
	a.arg.radians[1] = 0;
	a.arg.radians[2] = 2*M_PI;
	steplist.Append(a);
	a.arg.radians[0] = 0;
	a.arg.radians[1] = M_PI_2;
	a.arg.radians[2] = 0;
	steplist.Append(a);


	// animation message to status line
	SETTEXT(AnimateSolutionStatus);

	// start stage 
	stage = nbrstages+1;

	// set # of stages to completion
	nbrstages = 10;

	// make sure everyone knows we're animating
	bAnimatingSolution = TRUE;

	// set timer for every 1/10th second
	int r = SetTimer(HWindow,IDW_TIMER,100,NULL);

	// if no timers available, can't do anything
	if (!r) {
		MessageBox(HWindow, TimerErrorMsg, AppName, MB_ICONEXCLAMATION);
		while (steplist.Size()) delete steplist.RemoveHead();
	}
}

/*-----------------------------------------------------------------*
* This routine is actually a slave routine for the timer.         *
* Performs the requested animation of moving the currently        *
*   selected small cube to a place in the large cube.             *
* Assumes:  stage,nbrstages                                       *
*   (all TMainWindow private variables) have been properly set.   *
* Entry: Integer - index of point on large cube to move small     *
*                  cube to                                        *
* Return: None                                                    *
*-----------------------------------------------------------------*/
void TMainWindow::AnimatedMoveCube (int topoint)
{
	static double dx,dy;
	RTMessage msg;
	double percent;
	static int tp;

	if (!stage) {
		dx = (rlcDraw.left + lc.ps[topoint].x) - screct[ics].right;
		dy = (rlcDraw.top + lc.ps[topoint].y) - screct[ics].bottom;
		cmpos.x = mpos.x = (WORD)screct[ics].right-1;
		cmpos.y = mpos.y = (WORD)screct[ics].bottom-1;
		msg.lParam = MAKELPARAM(cmpos.x,cmpos.y);
		MoveCube(msg);
		tp = topoint;
	}
	percent = (double)stage/(double)nbrstages;
	msg.lParam = MAKELPARAM( mpos.x + (int)(percent * dx), mpos.y + (int)(percent * dy) );
	MoveCube(msg);
	if (++stage>nbrstages) {
		MoveCube(msg);
		DropCube();
	}
	return;
}


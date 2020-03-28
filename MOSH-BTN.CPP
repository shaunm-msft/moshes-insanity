#define _USE_MATH_DEFINES
#include <math.h>
#include <string.h>
#include "moshe.h"
#include "mosh-con.h"

/*--------------------------------------------------------------*
* Rotate the currently selected cube left                      *
* Entry:  Event message                                        *
* Return: None                                                 *
*--------------------------------------------------------------*/
void TMainWindow::TMainWindowIDW_BTNROTATELEFT (RTMessage Msg)
{
	if (bAnimatingSolution) return;

	// let user wait for animation
	EnableWindow(HWindow,FALSE);

	// Display rotation message
	SETTEXT("Rotating Left");
	UpdateWindow(hStatus);

	// setup the animation variables and let the
	// animation routine do the rest
	xrad = yrad = 0.0;
	zrad = -M_PI_2;
	AnimateRotation(10);
	return;
}

/*--------------------------------------------------------------*
* Rotate the currently selected cube right                     *
* Entry:  Event message                                        *
* Return: None                                                 *
*--------------------------------------------------------------*/
void TMainWindow::TMainWindowIDW_BTNROTATERIGHT (RTMessage Msg)
{
	if (bAnimatingSolution) return;

	// let user wait for animation
	EnableWindow(HWindow,FALSE);

	// Display rotation message
	SETTEXT("Rotating Right");
	UpdateWindow(hStatus);

	// setup the animation variables and let the
	// animation routine do the rest
	xrad = yrad = 0.0;
	zrad = M_PI_2;
	AnimateRotation(10);
	return;
}

/*--------------------------------------------------------------*
* Rotate the currently selected cube down                      *
* Entry:  Event message                                        *
* Return: None                                                 *
*--------------------------------------------------------------*/
void TMainWindow::TMainWindowIDW_BTNROTATEDOWN (RTMessage Msg)
{
	if (bAnimatingSolution) return;

	// let user wait for animation
	EnableWindow(HWindow,FALSE);

	// Display rotation message
	SETTEXT("Rotating Down");
	UpdateWindow(hStatus);

	// setup the animation variables and let the
	// animation routine do the rest
	zrad = xrad = 0.0;
	yrad = -M_PI_2;
	AnimateRotation(10);
	return;
}

/*--------------------------------------------------------------*
* Rotate the currently selected cube up                        *
* Entry:  Event message                                        *
* Return: None                                                 *
*--------------------------------------------------------------*/
void TMainWindow::TMainWindowIDW_BTNROTATEUP (RTMessage Msg)
{
	if (bAnimatingSolution) return;

	// let user wait for animation
	EnableWindow(HWindow,FALSE);

	// Display rotation message
	SETTEXT("Rotating Up");
	UpdateWindow(hStatus);

	// setup the animation variables and let the
	// animation routine do the rest
	zrad = xrad = 0.0;
	yrad = M_PI_2;
	AnimateRotation(10);
	return;
}

/*--------------------------------------------------------------*
* Rotate the currently selected cube's corner closest to the   *
*   viewpoint.                                                 *
* Entry:  Event message                                        *
* Return: None                                                 *
*--------------------------------------------------------------*/
void TMainWindow::TMainWindowIDW_BTNROTATECORNER (RTMessage Msg)
{
	if (bAnimatingSolution) return;

	// let user wait for animation
	EnableWindow(HWindow,FALSE);

	// Display rotation message
	SETTEXT("Rotating Corner");
	UpdateWindow(hStatus);

	// setup the animation variables and let the
	// animation routine do the rest
	zrad = M_PI_2;
	xrad = -M_PI_2;
	yrad = 0.0;
	AnimateRotation(10);
	return;
}


/********************************************************************
 *                                                                  *
 *   Source File: UsrWnd3D.cpp                                      *
 *                                                                  *
 *   This file contains the UserWindow3D class functions            *
 *   The UserWindow3D object handles all translation between the 3D *
 *   user window and the screen.                                    *
 *                                                                  *
 *   by Shaun Miller, 1994                                          *
 *                                                                  *
 ********************************************************************/



#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "window.h"
#include "usrwnd3d.h"


#ifndef min
	#define min(a,b)		(((a) < (b)) ? (a) : (b))
#endif
#ifndef max
	#define max(a,b)		(((a) > (b)) ? (a) : (b))
#endif


/*-----------------------------------------------------------------*
 * Constructor.  Zeros out the internal object data.  To use the   *
 *   window, call Init(...).                                       *
 * Entry:  None                                                    *
 * Return: None                                                    *
 *-----------------------------------------------------------------*/
UserWindow3D::UserWindow3D () : UserWindow()
{
	MinX = MaxX = MinY = MaxY = MinZ = MaxZ = 0;
	pt3dViewpoint.x = pt3dViewpoint.y = pt3dViewpoint.z = 0;
}

/*-----------------------------------------------------------------*
 * Window initialization.                                          *
 * Entry: Handle to window of screen viewport                      *
 *        Double, Double - min,max x coordinates of clipping cube  *
 *        Double, Double - min,max y coordinates of clipping cube  *
 *        Double, Double - min,max z coordinates of clipping cube  *
 *        Pointer to DPOINT3D - viewpoint (or point of perspective)*
 *        Integer (optional) - whether to show coordinate axes     *
 *                  default TRUE (ie. show axes)                   *
 *        COLORREF (optional) - foreground (border) color          *
 *                              default RGB(0,0,0)                 *
 *        COLORREF(optional) - background color                    *
 *                             default RGB(255,255,255)            *
 *        COLORREF (optional) - axes color, default RGB(0,0,0)     *
 * Return: None                                                    *
 *-----------------------------------------------------------------*/
void UserWindow3D::Init (HWND hwndScreen,
								double minx,double maxx,
								double miny, double maxy,
								double minz, double maxz,
								DPOINT3D *pt3dView,
								int bShowAxes,
								COLORREF crForeGround,
								COLORREF crBackGround,
								COLORREF crAxesColor)
{
		// Verify & store 3D drawing area
	MinX = min(minx,maxx);
	MaxX = max(minx,maxx);
	if (minx>0 && maxx>0) MinX = 0;
	if (minx<0 && maxx<0) MaxX = 0;
	MinY = min(miny,maxy);
   MaxY = max(miny,maxy);
	if (miny>0 && maxy>0) MinY = 0;
	if (miny<0 && maxy<0) MaxY = 0;
	MinZ = min(minz,maxz);
	MaxZ = max(minz,maxz);
	if (minz>0 && maxz>0) MinZ = 0;
	if (minz<0 && maxz<0) MaxZ = 0;

		// Store point of perspective (for projections)
	SetViewpoint(*pt3dView);

		// compute 2D user rect by projecting all 8 corners of the
		// drawing area to 2D and picking the smallest and largest
      // x and y values
	RECT ru;
   double bx,by,sx,sy;
	DPOINT p2d;
	DPOINT3D p3d;

	p3d.x = MaxX;
	p3d.y = MaxY;
   p3d.z = MaxZ;
	ProjectTo2D(p2d,p3d);
	bx = sx = p2d.x;
   by = sy = p2d.y;

	p3d.x = MaxX;
	p3d.y = MaxY;
   p3d.z = MinZ;
	ProjectTo2D(p2d,p3d);
	bx = max(bx,p2d.x);
	by = max(by,p2d.y);
	sx = min(sx,p2d.x);
	sy = min(sy,p2d.y);

	p3d.x = MaxX;
	p3d.y = MinY;
   p3d.z = MaxZ;
	ProjectTo2D(p2d,p3d);
	bx = max(bx,p2d.x);
	by = max(by,p2d.y);
	sx = min(sx,p2d.x);
	sy = min(sy,p2d.y);

	p3d.x = MaxX;
	p3d.y = MinY;
   p3d.z = MinZ;
	ProjectTo2D(p2d,p3d);
	bx = max(bx,p2d.x);
	by = max(by,p2d.y);
	sx = min(sx,p2d.x);
	sy = min(sy,p2d.y);

	p3d.x = MinX;
	p3d.y = MaxY;
   p3d.z = MaxZ;
	ProjectTo2D(p2d,p3d);
	bx = max(bx,p2d.x);
	by = max(by,p2d.y);
	sx = min(sx,p2d.x);
	sy = min(sy,p2d.y);

	p3d.x = MinX;
	p3d.y = MaxY;
   p3d.z = MinZ;
	ProjectTo2D(p2d,p3d);
	bx = max(bx,p2d.x);
	by = max(by,p2d.y);
	sx = min(sx,p2d.x);
	sy = min(sy,p2d.y);

	p3d.x = MinX;
	p3d.y = MinY;
   p3d.z = MaxZ;
	ProjectTo2D(p2d,p3d);
	bx = max(bx,p2d.x);
	by = max(by,p2d.y);
	sx = min(sx,p2d.x);
	sy = min(sy,p2d.y);

	p3d.x = MinX;
	p3d.y = MinY;
   p3d.z = MinZ;
	ProjectTo2D(p2d,p3d);
	bx = max(bx,p2d.x);
	by = max(by,p2d.y);
	sx = min(sx,p2d.x);
	sy = min(sy,p2d.y);

	ru.left = (LONG)sx;
	ru.right = (LONG)bx;
	ru.top = (LONG)by;
	ru.bottom = (LONG)sy;

	if (ru.right <= ru.left) ru.right = ru.left+1;
	if (ru.bottom >= ru.top) ru.bottom = ru.top-1;

	UserWindow::Init(hwndScreen,&ru,bShowAxes,crForeGround,
		crBackGround,crAxesColor);

	return;
}

/*-----------------------------------------------------------------*
 * Retrieve user window cube structure.                            *
 * Entry: 6 Pointers to Double                                     *
 * Return: None                                                    *
 *-----------------------------------------------------------------*/
void UserWindow3D::GetUserWindowRect3D (double *minx,double *maxx,
			double *miny, double *maxy, double *minz, double *maxz) const
{
	if (minx) *minx = MinX;
	if (maxx) *maxx = MaxX;
	if (miny) *miny = MinY;
	if (maxy) *maxy = MaxY;
	if (minz) *minz = MinZ;
	if (maxz) *maxz = MaxZ;
	return;
}

/*-----------------------------------------------------------------*
 * Translate a user window point to a screen viewport point.       *
 * Entry: POINT reference - destination viewport point structure   *
 *        DPOINT3D reference - source user window point structure  *
 * Return: None                                                    *
 *-----------------------------------------------------------------*/
void UserWindow3D::TranslateToScreenPoint (POINT &ptScreen,
	const DPOINT3D &ptUser) const
{
	if (GetStatus() != UW_OK) return;

	DPOINT p2d;
	ProjectTo2D(p2d,ptUser);
   UserWindow::TranslateToScreenPoint(ptScreen,p2d);

	return;
}

/*-----------------------------------------------------------------*
 * Projects a 3D point to a 2D point (all real number coords).     *
 * Entry: DPOINT reference - destination 2D point structure        *
 *        DPOINT3D reference - source 3D point structure           *
 * Return: None                                                    *
 *-----------------------------------------------------------------*/
void UserWindow3D::ProjectTo2D (DPOINT &p2d, const DPOINT3D &p3d) const
{
	double xp = (p3d.x * costheta * sinphi)
					+ (p3d.y * sintheta * sinphi)
					+ (p3d.z * cosphi);
	double yp = (-p3d.x * sintheta) + (p3d.y * costheta);
	double zp = (-p3d.x * costheta * cosphi)
					- (p3d.y * sintheta * cosphi)
					+ (p3d.z * sinphi);
	double d;
	if (R+xp == 0.0) {
		RECT ru;
		GetUserWindowRect(&ru);
		p2d.x = ru.right + 1;
		p2d.y = ru.bottom + 1;
		return;
   }

	d = R/(R-xp);

		// finally calculate the 2D coordinates
	p2d.x = yp * d;
	p2d.y = zp * d;

	return; 
}

/*-----------------------------------------------------------------*
 * Sets a pixel on the screen viewport (window).                   *
 * Note:  This hides the SetPixel function in the base class       *
 *          UserWindow.  To use that function call                 *
 *          uwnd3d.UserWindow::SetPixel(DPOINT,int)                *
 * Entry: DPOINT3D reference - point to set                        *
 *        Integer (optional) - update display now (don't wait for  *
 *                             WM_PAINT event)  default TRUE       *
 * Return: None                                                    *
 *-----------------------------------------------------------------*/
void UserWindow3D::SetPixel (const DPOINT3D &ptUser, const int bUpdate)
{
	DPOINT p2d;

	if (GetStatus() != UW_OK) return;

	ProjectTo2D(p2d,ptUser);
	UserWindow::SetPixel(p2d,bUpdate);

   return;
}

/*-----------------------------------------------------------------*
 * Draw the coordinate axes on the display (screen viewport).      *
 * Entry: None                                                     *
 * Return: None                                                    *
 *-----------------------------------------------------------------*/
void UserWindow3D::DrawAxes ()
{
	DPOINT3D p3d;
	POINT pOrigin,p;
	HDC hdcMemScreen = MemScreenDC();
	HBRUSH hbrAxes = CreateSolidBrush(GetAxesColor());
	SelectObject(hdcMemScreen,hbrAxes);

   	// find origin on viewport
	p3d.x = 0;
	p3d.y = 0;
   p3d.z = 0;
	TranslateToScreenPoint(pOrigin,p3d);

		// x-axis
	MoveTo(hdcMemScreen,pOrigin.x,pOrigin.y);
	p3d.x = MaxX;
	p3d.y = 0;
   p3d.z = 0;
	TranslateToScreenPoint(p,p3d);
   LineTo(hdcMemScreen,p.x,p.y);

		// y-axis
	MoveTo(hdcMemScreen,pOrigin.x,pOrigin.y);
	p3d.x = 0;
	p3d.y = MaxY;
   p3d.z = 0;
	TranslateToScreenPoint(p,p3d);
   LineTo(hdcMemScreen,p.x,p.y);

		// z-axis
	MoveTo(hdcMemScreen,pOrigin.x,pOrigin.y);
	p3d.x = 0;
	p3d.y = 0;
   p3d.z = MaxZ;
	TranslateToScreenPoint(p,p3d);
   LineTo(hdcMemScreen,p.x,p.y);

   	// cleanup;
	DeleteDC(hdcMemScreen);
	DeleteObject(hbrAxes);
}

/*-----------------------------------------------------------------*
 * Changes the point of perspective (viewpoint) for projections to *
 *   2D.  Updates associated data required for projections to 2D.  *
 * Entry: DPOINT3D reference - new viewpoint                       *
 * Return: None                                                    *
 *-----------------------------------------------------------------*/
void UserWindow3D::SetViewpoint (const DPOINT3D &p)
{
	memmove(&pt3dViewpoint,&p,sizeof(DPOINT3D));

	double x2,y2,z2;
	x2 = pt3dViewpoint.x * pt3dViewpoint.x;
	y2 = pt3dViewpoint.y * pt3dViewpoint.y;
	z2 = pt3dViewpoint.z * pt3dViewpoint.z;

	R = sqrt(x2+y2+z2);
	costheta = pt3dViewpoint.x/sqrt(x2+y2);
	sintheta = pt3dViewpoint.y/sqrt(x2+y2);
	cosphi = pt3dViewpoint.z/sqrt(x2+y2+z2);
	sinphi = sqrt(x2+y2)/sqrt(x2+y2+z2);

	return;
}

/*-----------------------------------------------------------------*
 * Retrieves the point of perspective (viewpoint) for projections  *
 *   to 2D.                                                        *
 * Entry: DPOINT3D pointer - viewpoint receptical                  *
 * Return: None                                                    *
 *-----------------------------------------------------------------*/
void UserWindow3D::GetViewpoint (DPOINT3D *p) const
{
	if (p) memmove(p,&pt3dViewpoint,sizeof(DPOINT3D));
	return;
}

/*-----------------------------------------------------------------*
 * 3D rotation.                                                    *
 * This is NOT a member function, but logically goes here anyway.  *
 * Entry: DPOINT3D reference - point to rotate                     *
 *        Double - radians to rotate in the x direction            *
 *        Double - radians to rotate in the y direction            *
 *        Double - radians to rotate in the z direction            *
 * Return: None                                                    *
 *-----------------------------------------------------------------*/
void Rotate3D (DPOINT3D &p, double xRadians, double yRadians,
	double zRadians)
{
	double sint, cost;
	double xt,yt,zt;

	if (xRadians != 0.0) {
		sint = sin(xRadians);
		cost = cos(xRadians);
		yt = p.y * cost - p.z * sint;
		zt = p.y * sint + p.z * cost;
		p.y = yt;
		p.z = zt;
	}
	if (yRadians != 0.0) {
		sint = sin(yRadians);
		cost = cos(yRadians);
		zt = p.z * cost - p.x * sint;
		xt = p.z * sint + p.x * cost;
		p.x = xt;
		p.z = zt;
	} 
	if (zRadians != 0.0) {
		sint = sin(zRadians);
		cost = cos(zRadians);
		xt = p.x * cost - p.y * sint;
		yt = p.x * sint + p.y * cost;
		p.x = xt;
		p.y = yt;
	}
	return;
}

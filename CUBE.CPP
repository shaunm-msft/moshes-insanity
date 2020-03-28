/********************************************************************
 *                                                                  *
 *   Source File: Cube.cpp                                          *
 *                                                                  *
 *   A cube derived from the Shape3D base class.						  *
 *                                                                  *
 *   by Shaun Miller, 1994                                          *
 *                                                                  *
 ********************************************************************/


#include <windows.h>
#include "window.h"
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "cube.h"



/**************************
 * Implementation of Cube *
 **************************/

/*-----------------------------------------------------------------*
 * Draws a cube on the specified window.  Depending on the         *
 *   options, the cube's background can be made transparent.       *
 * Entry:  3D user window                                          *
 *         Integer (optional) - TRUE/FALSE whether to make the     *
 *           background tranparent and combine with the background *
 *           on the 3D user window.  default is FALSE              *
 *         HDC (optional) - DC of background to combine with the   *
 *           transparent background of the drawn cube              *
 *         Integer (optional) - width/height of DC background. not *
 *           to be used with the 2nd parameter                     *
 * Return: None                                                    *
 *-----------------------------------------------------------------*/
void Cube::Draw (UserWindow3D &uwnd,
	const int bCombineWithUserWindowBackground,
	HDC hdcBackground, int width_height)
{
	int i;
   int bDeleteBackground = 0;
	HBITMAP hbmpBackground;
	int bx, by, bw, bh;

		// update public cube data to match internal cube data
	UpdatePoints(uwnd);

		// get device context to drawn on
	HDC hdc = uwnd.MemScreenDC();

		// select border color
   HPEN newpen = CreatePen(PS_SOLID,1,BorderColor());
	SelectObject(hdc,newpen);

		// find size & origin of background of cube
	if (bCombineWithUserWindowBackground) {

			// find background size & origin
		int minx,miny,maxx,maxy;
		minx = maxx = ps[0].x;
		miny = maxy = ps[0].y;
		for (int i=1; i<8; i++) {
			if (ps[i].x < minx) minx = ps[i].x;
			else if (ps[i].x > maxx) maxx = ps[i].x;
			if (ps[i].y < miny) miny = ps[i].y;
			else if (ps[i].y > maxy) maxy = ps[i].y;
		}
		bx = minx;
		by = miny;
		bw = maxx-minx+1;
		bh = maxy-miny+1;
	}
	else {
		bx = by = 0;
		bw = bh = width_height;
   }

		// get background of user window
	if (bCombineWithUserWindowBackground && !hdcBackground) {
		hdcBackground = CreateCompatibleDC(hdc);
		hbmpBackground = CreateCompatibleBitmap(hdc,bw,bh);
		SelectObject(hdcBackground,hbmpBackground);
		BitBlt(hdcBackground,0,0,bw,bh, hdc,bx,by, SRCCOPY);
		bDeleteBackground = 1;
   }
        

	// Drawing sequence:
	// 0 -> 2,3,4
	// 1 -> 2,3,5
	// 6 -> 2,4,5
	// 7 -> 3,4,5
   	
		// if we are combining the drawn cube with some sort of
		// background, then we need to first mask out the area
		// where the cube will be drawn, then draw the cube
		// this requires that the cube be drawn and colored
		// twice - once with the mask colors and once with the
      // real colors
	if (bHiddenFaces) {
		if (hdcBackground) {
			SelectObject(hdc,GetStockObject(WHITE_PEN));
			SelectObject(hdc,GetStockObject(WHITE_BRUSH));
			PatBlt(hdc,bx,by,bw,bh,BLACKNESS);
				// 0 -> 2,3,4
			if (!bHidePoint[0]) {
				MoveTo(hdc,ps[0].x,ps[0].y);
				if (!bHidePoint[2]) LineTo(hdc,ps[2].x,ps[2].y);
				MoveTo(hdc,ps[0].x,ps[0].y);
				if (!bHidePoint[3]) LineTo(hdc,ps[3].x,ps[3].y);
				MoveTo(hdc,ps[0].x,ps[0].y);
				if (!bHidePoint[4]) LineTo(hdc,ps[4].x,ps[4].y);
		   }
				// 1 -> 2,3,5
			if (!bHidePoint[1]) {
				MoveTo(hdc,ps[1].x,ps[1].y);
				if (!bHidePoint[2]) LineTo(hdc,ps[2].x,ps[2].y);
				MoveTo(hdc,ps[1].x,ps[1].y);
				if (!bHidePoint[3]) LineTo(hdc,ps[3].x,ps[3].y);
				MoveTo(hdc,ps[1].x,ps[1].y);
				if (!bHidePoint[5]) LineTo(hdc,ps[5].x,ps[5].y);
   		}
				// 6 -> 2,4,5
			if (!bHidePoint[6]) {
				MoveTo(hdc,ps[6].x,ps[6].y);
				if (!bHidePoint[2]) LineTo(hdc,ps[2].x,ps[2].y);
				MoveTo(hdc,ps[6].x,ps[6].y);
				if (!bHidePoint[4]) LineTo(hdc,ps[4].x,ps[4].y);
				MoveTo(hdc,ps[6].x,ps[6].y);
				if (!bHidePoint[5]) LineTo(hdc,ps[5].x,ps[5].y);
	   	}
				// 7 -> 3,4,5
			if (!bHidePoint[7]) {
				MoveTo(hdc,ps[7].x,ps[7].y);
				if (!bHidePoint[3]) LineTo(hdc,ps[3].x,ps[3].y);
				MoveTo(hdc,ps[7].x,ps[7].y);
				if (!bHidePoint[4]) LineTo(hdc,ps[4].x,ps[4].y);
				MoveTo(hdc,ps[7].x,ps[7].y);
				if (!bHidePoint[5]) LineTo(hdc,ps[5].x,ps[5].y);
			}

	      	// insure all corner points are colored
			for (i=0; i<8; i++) {
				if (!bHidePoint[i]) {
					SetPixel(hdc,ps[i].x,ps[i].y,RGB(255,255,255));
            }
			}

				// color faces white
			for (i=0; i<6; i++) {
				if (!bHideFace[i]) {
					FloodFill(hdc,
              		(ps[fp[i][0]].x + ps[fp[i][2]].x)/2,
						(ps[fp[i][0]].y + ps[fp[i][2]].y)/2,
						RGB(255,255,255)
					);
				}
			}
         BitBlt(hdc,bx,by,bw,bh,hdcBackground,0,0,SRCPAINT);
			if (bDeleteBackground) {
				DeleteDC(hdcBackground);
				DeleteObject(hbmpBackground);
			}
			SelectObject(hdc,newpen);
		} // end combine background

			// 0 -> 2,3,4
		if (!bHidePoint[0]) {
			MoveTo(hdc,ps[0].x,ps[0].y);
			if (!bHidePoint[2]) LineTo(hdc,ps[2].x,ps[2].y);
			MoveTo(hdc,ps[0].x,ps[0].y);
			if (!bHidePoint[3]) LineTo(hdc,ps[3].x,ps[3].y);
			MoveTo(hdc,ps[0].x,ps[0].y);
			if (!bHidePoint[4]) LineTo(hdc,ps[4].x,ps[4].y);
	   }
			// 1 -> 2,3,5
		if (!bHidePoint[1]) {
			MoveTo(hdc,ps[1].x,ps[1].y);
			if (!bHidePoint[2]) LineTo(hdc,ps[2].x,ps[2].y);
			MoveTo(hdc,ps[1].x,ps[1].y);
			if (!bHidePoint[3]) LineTo(hdc,ps[3].x,ps[3].y);
			MoveTo(hdc,ps[1].x,ps[1].y);
			if (!bHidePoint[5]) LineTo(hdc,ps[5].x,ps[5].y);
   	}
			// 6 -> 2,4,5
		if (!bHidePoint[6]) {
			MoveTo(hdc,ps[6].x,ps[6].y);
			if (!bHidePoint[2]) LineTo(hdc,ps[2].x,ps[2].y);
			MoveTo(hdc,ps[6].x,ps[6].y);
			if (!bHidePoint[4]) LineTo(hdc,ps[4].x,ps[4].y);
			MoveTo(hdc,ps[6].x,ps[6].y);
			if (!bHidePoint[5]) LineTo(hdc,ps[5].x,ps[5].y);
   	}
			// 7 -> 3,4,5
		if (!bHidePoint[7]) {
			MoveTo(hdc,ps[7].x,ps[7].y);
			if (!bHidePoint[3]) LineTo(hdc,ps[3].x,ps[3].y);
			MoveTo(hdc,ps[7].x,ps[7].y);
			if (!bHidePoint[4]) LineTo(hdc,ps[4].x,ps[4].y);
			MoveTo(hdc,ps[7].x,ps[7].y);
			if (!bHidePoint[5]) LineTo(hdc,ps[5].x,ps[5].y);
		}

      	// insure all corner points are colored
		COLORREF border = BorderColor();
      int i;
		for (i=0; i<8; i++) {
			if (!bHidePoint[i]) SetPixel(hdc,ps[i].x,ps[i].y,border);
		}

			// color faces
		if (bColorFaces) {
			HBRUSH hbrnew;
			HGDIOBJ hbrorig;
         int tf=0;
			for (i=0; i<6; i++) {
				if (!bHideFace[i]) {
					hbrnew = CreateSolidBrush(crFace[i]);
					if (!tf) {
						tf=1;
						hbrorig = SelectObject(hdc,hbrnew);
					}
               else DeleteObject(SelectObject(hdc,hbrnew));
					FloodFill(hdc,
               	(ps[fp[i][0]].x + ps[fp[i][2]].x)/2,
						(ps[fp[i][0]].y + ps[fp[i][2]].y)/2,
						border
					);
				}
			}
			DeleteObject(SelectObject(hdc,hbrorig));
      }

	}
   else {

			// Draw base rectangle
		MoveTo(hdc,ps[0].x,ps[0].y);
		LineTo(hdc,ps[2].x,ps[2].y);
		LineTo(hdc,ps[1].x,ps[1].y);
		LineTo(hdc,ps[3].x,ps[3].y);
		LineTo(hdc,ps[0].x,ps[0].y);
	
   		// Draw top rectangle
		MoveTo(hdc,ps[4].x,ps[4].y);
		LineTo(hdc,ps[6].x,ps[6].y);
		LineTo(hdc,ps[5].x,ps[5].y);
		LineTo(hdc,ps[7].x,ps[7].y);
		LineTo(hdc,ps[4].x,ps[4].y);

			// Draw line connecting the two rectangles
		MoveTo(hdc,ps[0].x,ps[0].y);
		LineTo(hdc,ps[4].x,ps[4].y);
		MoveTo(hdc,ps[1].x,ps[1].y);
		LineTo(hdc,ps[5].x,ps[5].y);
		MoveTo(hdc,ps[6].x,ps[6].y);
		LineTo(hdc,ps[2].x,ps[2].y);
		MoveTo(hdc,ps[7].x,ps[7].y);
		LineTo(hdc,ps[3].x,ps[3].y);
	}

		// cleanup
	DeleteDC(hdc);
	DeleteObject(newpen);
   	
		// refresh screen
	uwnd.Refresh();
	return;
}

/*-----------------------------------------------------------------*
 * Computes the hidden faces and points of a cube on a given       *
 *   3D user window.                                               *
 * Entry:  3D user window                                          *
 * Return: None                                                    *
 *-----------------------------------------------------------------*/
void Cube::HidePoints (UserWindow3D &u)
{
	DPOINT3D v;
	double A,B,C,X1,Y1,Z1,X2,Y2,Z2;
   int f,p;

   	// get point of perspective
	u.GetViewpoint(&v);

	for (f=0; f<6; f++) {
			// compute A,B,C of plane equation
		A = (pr[fp[f][2]].y-pr[fp[f][1]].y)*(pr[fp[f][3]].z-pr[fp[f][1]].z)
			-(pr[fp[f][3]].y-pr[fp[f][1]].y)*(pr[fp[f][2]].z-pr[fp[f][1]].z);
		B = (pr[fp[f][2]].x-pr[fp[f][1]].x)*(pr[fp[f][3]].z-pr[fp[f][1]].z)
			-(pr[fp[f][3]].x-pr[fp[f][1]].x)*(pr[fp[f][2]].z-pr[fp[f][1]].z);
		C = (pr[fp[f][2]].x-pr[fp[f][1]].x)*(pr[fp[f][3]].y-pr[fp[f][1]].y)
			-(pr[fp[f][3]].x-pr[fp[f][1]].x)*(pr[fp[f][2]].y-pr[fp[f][1]].y);

			// compute x,y,z of plane equation for both viewpoint
         // and center of cube
		X1 = offset.x - pr[fp[f][1]].x;
		Y1 = offset.y - pr[fp[f][1]].y;
		Z1 = offset.z - pr[fp[f][1]].z;
		X2 = v.x - pr[fp[f][1]].x;
		Y2 = v.y - pr[fp[f][1]].y;
		Z2 = v.z - pr[fp[f][1]].z;

			// if the signs of the equations are different, then
			// the points are on different sides of the plane and
         // the plane is visible
		if ((A*X1-B*Y1+C*Z1)*(A*X2-B*Y2+C*Z2) < 0.0) bHideFace[f] = 0;
		else bHideFace[f] = 1;
	}

		// compute which points are hidden by assuming all points
      // are hidden, then unhiding all points on the visible faces
   memset(bHidePoint,1,8*sizeof(int));
	for (f=0; f<6; f++) {
   	if (!bHideFace[f]) {
			for (p=0; p<4; p++) bHidePoint[fp[f][p]]=0;
		}
	}

	return;
}

/*-----------------------------------------------------------------*
 * Updates public cube data to match internal cube data.  This     *
 *   means that it translates the cube from being centered at the  *
 *   origin (where all computations on the cube such as rotation   *
 *   are done) to its original location; it also projects the      *
 *   points into screen coordinates; it also computes the hidden   *
 *   points and faces.                                             *
 * Entry:  3D user window                                          *
 * Return: None                                                    *
 *-----------------------------------------------------------------*/
void Cube::UpdatePoints (UserWindow3D &u)
{
	for (int i=0; i<8; i++) {
		pr[i].x = po[i].x + offset.x;
		pr[i].y = po[i].y + offset.y;
		pr[i].z = po[i].z + offset.z;
		u.TranslateToScreenPoint(ps[i],pr[i]);
	}
	if (bHiddenFaces) HidePoints(u);
	return;
}

/*-----------------------------------------------------------------*
 * Rotates a cube through the specified angles in the x,y,z        *
 *   directions.                                                   *
 * Entry:  Double - radians to rotate in the x direction           *
 *         Double - radians to rotate in the y direction           *
 *         Double - radians to rotate in the z direction           *
 * Return: None                                                    *
 *-----------------------------------------------------------------*/
void Cube::Rotate (double xRadians, double yRadians, double zRadians)
{
	for (int i=0; i<8; i++) Rotate3D(po[i],xRadians,yRadians,zRadians);
	return;
}   	

/*-----------------------------------------------------------------*
 * Stores the base rectangle and height of the cube.  Then         *
 *   calculates the rest of the corner points of the cube, its     *
 *   offset from the origin, and the corner points of the cube     *
 *   centered at the origin.                                       *
 * Entry:  3D Point - 1st point of base rectangle of cube          *
 *         3D Point - 2nd point of base rectangle of cube          *
 *         Double - height of cube                                 *
 * Return: None                                                    *
 *-----------------------------------------------------------------*/
void Cube::SetEquation (const DPOINT3D &p1i, const DPOINT3D &p2i,
	const double height)
{
		// store the base rectangle and height
	memmove(&pr[0],&p1i,sizeof(DPOINT3D));
	memmove(&pr[1],&p2i,sizeof(DPOINT3D));
	h = height;

		// compute the rest of the base rectangle
	pr[2].x = pr[0].x;
	pr[2].y = pr[1].y;
	pr[2].z = pr[0].z;
	pr[3].x = pr[1].x;
	pr[3].y = pr[0].y;
	pr[3].z = pr[1].z;

		// copy the base rectangle but h units above the original
	double dxyz = sqrt(
						(pr[1].x-pr[0].x)*(pr[1].x-pr[0].x)
						+ (pr[1].y-pr[0].y)*(pr[1].y-pr[0].y)
						+ (pr[1].z-pr[0].z)*(pr[1].z-pr[0].z)
					);
	double dxy = sqrt(
						(pr[1].x-pr[0].x)*(pr[1].x-pr[0].x)
						+ (pr[1].y-pr[0].y)*(pr[1].y-pr[0].y)
					);
	pr[4].x = pr[0].x;
	pr[4].y = pr[0].y + h*(pr[1].z-pr[0].z)/dxyz;
	pr[4].z = pr[0].z + h*dxy/dxyz;
	pr[5].x = pr[1].x;
	pr[5].y = pr[1].y + h*(pr[1].z-pr[0].z)/dxyz;
	pr[5].z = pr[1].z + h*dxy/dxyz;
	pr[6].x = pr[4].x;
	pr[6].y = pr[5].y;
	pr[6].z = pr[4].z;
	pr[7].x = pr[5].x;
	pr[7].y = pr[4].y;
	pr[7].z = pr[5].z;

		// compute cube center's offset from origin
	offset.x = (pr[0].x + pr[5].x)/2.0;
	offset.y = (pr[0].y + pr[5].y)/2.0;
	offset.z = (pr[0].z + pr[5].z)/2.0;

		// compute cube centered at origin
   int i;
	for (i=0; i<8; i++) {
		po[i].x = pr[i].x - offset.x;
		po[i].y = pr[i].y - offset.y;
		po[i].z = pr[i].z - offset.z;
	}

	return;
}

/*-----------------------------------------------------------------*
 * Constructor.  Defaults the cube to no hidden points and calls   *
 *   SetEquation                                                   *
 * Entry:  3D Point - 1st point of base rectangle of cube          *
 *         3D Point - 2nd point of base rectangle of cube          *
 *         Double - height of cube                                 *
 *         COLORREF (optional) - border color.  default RGB(0,0,0) *
 * Return: None                                                    *
 *-----------------------------------------------------------------*/
Cube::Cube (DPOINT3D &p1i, DPOINT3D &p2i,
	double height, COLORREF border) : Shape3D (border)
{
	bHiddenFaces = 0;
	SetEquation (p1i,p2i,height);
}

/*-----------------------------------------------------------------*
 * Default Constructor.  Defaults the cube to no hidden points     *
 * Entry:  None                                                    *
 * Return: None                                                    *
 *-----------------------------------------------------------------*/
Cube::Cube ()
{
	bHiddenFaces = 0;
}

/*-----------------------------------------------------------------*
 * Finds the index of a point given the coordinates of the point   *
 *   on the cube centered at the origin.  If the point is on the   *
 *   cube centered at the origin, first add the offset to its      *
 *   original location before calling this function.               *
 * Entry:  3D Point - coordinates of the point                     *
 * Return: Integer - index of closest point within tolerance       *
 *                   8th point if there is no such point           *
 *-----------------------------------------------------------------*/
int Cube::IndexFromPoint (const DPOINT3D &p) const
{
		// tolerance is 1/4 the radius of the cube
	double tolerance =
		sqrt(po[0].x*po[0].x + po[0].y*po[0].y + po[0].z*po[0].z)/4;

	int i;
	for (i=0; i<7; i++) {
		if ( fabs(p.x - pr[i].x) < tolerance
			&& fabs(p.y - pr[i].y) < tolerance
			&& fabs(p.z - pr[i].z) < tolerance)
		{
			break;
		}
	}
	return i;
}

/*-----------------------------------------------------------------*
 * Finds the first face containing the 2 given indices of two      *
 *   corner points on the cube.                                    *
 * Entry:  Integer - index of the 1st point                        *
 *         Integer - index of the 2nd point                        *
 * Return: Integer - index of face                                 *
 *                   6th face if there is no such face             *
 *-----------------------------------------------------------------*/
// find the first face containing the 2 given the indices to two points
int Cube::FaceFromPointIndices (const int p1, const int p2) const
{
	int i,j,f1,f2;
	for (i=0; i<5; i++) {
   	f1 = f2 = 0;
		for (j=0; j<4; j++) {
			if (fp[i][j]==p1) f1++;
			if (fp[i][j]==p2) f2++;
		}
      if (f1 && f2) break;;
	}
	return i;
}

/*-----------------------------------------------------------------*
 * Assignment of cube static member. Points defining each face of  *
 *   the cube.  The order of the faces and the points is important *
 *   to the routine that colors the faces.                         *
 *-----------------------------------------------------------------*/
const int Cube::fp[6][4] = {
	{3,1,5,7}, {3,0,4,7}, {0,2,6,4}, {2,1,5,6}, {4,6,5,7},{0,2,1,3},
};


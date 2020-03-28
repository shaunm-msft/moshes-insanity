/********************************************************************
 *                                                                  *
 *   Source File: UsrWnd3D.h                                        *
 *                                                                  *
 *   This file contains the UserWindow3D class header               *
 *   The UserWindow object handles all translation between the 3D   * 
 *   user window and the screen.                                    *
 *                                                                  *
 *   by Shaun Miller, 1994                                          *
 *                                                                  *
 ********************************************************************/



#ifndef __USERWIND3D__

#define __USERWIND3D__
#include <windows.h>
#include "userwind.h"

// declare double precision 3D point structure
typedef struct {
	double x,y,z;
} DPOINT3D;


class UserWindow3D : public UserWindow {
	private:
			// bounding cube of user (imaginary) window
		double MinX, MaxX;
		double MinY, MaxY;
		double MinZ, MaxZ;
			// viewpoint of projection to 2D
		DPOINT3D pt3dViewpoint;
			// calculated fields needed for projection to 2D
			// result of the viewpoint given as polar coordinates:
			//   (R,theta,phi)
		double R;
		double costheta;
		double sintheta;
		double cosphi;
		double sinphi;


	public:
			// See the .cpp file for descriptions if it isn't clear here!

		void GetUserWindowRect3D (double *minx,double *maxx,
			double *miny, double *maxy, double *minz, double *maxz) const;
		void GetViewpoint (DPOINT3D *p) const;
		void SetViewpoint (const DPOINT3D &r);

      	// projection & translation
		void ProjectTo2D (DPOINT &p2d, const DPOINT3D &p3d) const;
		void TranslateToScreenPoint (POINT &ptScreen,
			const DPOINT3D &ptUser) const;

		void SetPixel (const DPOINT3D &ptUser, const int bUpdate=TRUE);
			// draws axes on screen viewport
		virtual void DrawAxes();

		void Init (
			HWND hwndScreenViewport,
			double minx,double maxx,
			double miny, double maxy,
			double minz, double maxz,
			DPOINT3D *viewpoint,
			int bShowAxes = TRUE,
			COLORREF crForeGround = RGB(0,0,0),
			COLORREF crBackGround = RGB(255,255,255),
			COLORREF crAxesColor = RGB(0,0,0)
		);

		UserWindow3D ();
};

// 3D rotation
void Rotate3D (DPOINT3D &p, double xRadians, double yRadians, double zRadians);


#endif
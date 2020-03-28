/********************************************************************
 *                                                                  *
 *   Source File: Cube.h                                            *
 *                                                                  *
 *   by Shaun Miller, 1994                                          *
 *                                                                  *
 ********************************************************************/

/*

Cube.h
by Shaun Miller

A cube derived from the Shape3D base class.

*/

#ifndef __CUBE__
#define __CUBE__

#include <windows.h>
#include "shape3d.h"
#include "usrwnd3d.h"

class Cube : public Shape3D
{
	public:
			// 8 pts composing the cube translated so the center of the
         // cube is at the origin
			// 	0-4 are the base rectangle
			// 	5-8 are the top rectangle
		DPOINT3D po[8];

			// height of cube
		double h;

			// cube center's offset from origin
		DPOINT3D offset;

			// whether to hide the point or not;
		int bHidePoint[8];

			// whether to hide the point or not;
		int bHideFace[6];

			// indexes to points composing each face
		const static int fp[6][4];

			// cube's real coordinates
		DPOINT3D pr[8];

			// cube's screen coordinates
		POINT ps[8];

			// whether to hide anything
		int bHiddenFaces;

			// colors of the cube faces in the following order:
			//   four sides starting with front face and listed
			//     in clockwise order looking from the top.
			//   top
			//   bottom
		COLORREF crFace[6];

			// whether to color the faces
			// Note that it makes no sense to color non-hidden faces
         // so bHiddenFaces must be TRUE, otherwise this is ignored.
		int bColorFaces;

	public:
   		// computes hidden points & faces
		void HidePoints (UserWindow3D &u);

      	// updates the pr, ps, hiddenXXX data structures
		void UpdatePoints (UserWindow3D &u);

      	// rotates the cube
		void Rotate (double xRadians, double yRadians, double zRadians);

      	// draws the cube
		void Draw (UserWindow3D &u,
			const int bCombineWithUserWindowBackground=FALSE,
			HDC hdcBackground=NULL, int width_height=1);

         // set the equation of the cube
		void SetEquation (const DPOINT3D &p1i, const DPOINT3D &p2i,
			const double height);

         // retrieve the equation of the cube
		void GetEquation (DPOINT3D *p1i, DPOINT3D *p2i,
			double *height) const;

         // constructors
		Cube (DPOINT3D &p1i, DPOINT3D &p2i,
			double height, COLORREF border=RGB(0,0,0));
		Cube ();

      	// find index of closest corner point to a point
		int IndexFromPoint (const DPOINT3D &p) const;

			// find index of face given indices of two corner points
         // of a face of the cube
		int FaceFromPointIndices (const int p1, const int p2) const;
};

#endif

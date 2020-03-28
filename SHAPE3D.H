/********************************************************************
 *                                                                  *
 *   Source File: Shape3D.h                                         *
 *   This file contains the Shape3D base class header               *
 *                                                                  *
 *   by Shaun Miller, 1994                                          *
 *                                                                  *
 ********************************************************************/


#ifndef __Shape3D__
#define __Shape3D__

#include <windows.h>

class Shape3D {
	private:
		COLORREF crBorder;
		COLORREF crFill;

	public:
		COLORREF BorderColor (const COLORREF border);
		COLORREF FillColor (const COLORREF fill);
		COLORREF BorderColor (void);
		COLORREF FillColor (void);

		Shape3D (COLORREF border=RGB(0,0,0), COLORREF fill=RGB(255,255,255));
		~Shape3D () {};
};

#endif

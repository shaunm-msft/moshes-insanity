/********************************************************************
 *                                                                  *
 *   Source File: CCR.h                                             *
 *                                                                  *
 *   Merely a structure to describe a cubes corner and rotation.    *
 *                                                                  *
 *   by Shaun Miller, 1994                                          *
 *                                                                  *
 ********************************************************************/

class CCR {
	public:
		int cube,corner,rotation;
		CCR();
		CCR(const CCR&);
		~CCR();
		CCR& operator = (const CCR&);
		int operator == (CCR&);
		int operator != (CCR&);
};


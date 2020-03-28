/********************************************************************
 *                                                                  *
 *   Source File: Shape3D.cpp                                       *
 *   This file contains the Shape3D base class header functions     *
 *                                                                  *
 *   by Shaun Miller, 1994                                          *
 *                                                                  *
 ********************************************************************/


#include "Shape3D.h"

/*--------------------------------------------------------------*
 * Set border color.                                            *
 * Entry:  COLORREF - border color                              *
 * Return: COLORREF - previous border color                     *
 *--------------------------------------------------------------*/
COLORREF Shape3D::BorderColor (const COLORREF border)
{
	COLORREF c = crBorder;
	crBorder = border;
	return c;
}

/*--------------------------------------------------------------*
 * Set fill color.                                              *
 * Entry:  COLORREF - fill color                                *
 * Return: COLORREF - previous fill color                       *
 *--------------------------------------------------------------*/
COLORREF Shape3D::FillColor (const COLORREF fill)
{
	COLORREF c = crFill;
	crFill = fill;
	return c;
}

/*--------------------------------------------------------------*
 * Retrieve border color.                                       *
 * Entry:  None                                                 *
 * Return: COLORREF - border color                              *
 *--------------------------------------------------------------*/
COLORREF Shape3D::BorderColor (void)
{
	return crBorder;
}

/*--------------------------------------------------------------*
 * Retrieve fill color.                                         *
 * Entry:  None                                                 *
 * Return: COLORREF - fill color                                *
 *--------------------------------------------------------------*/
COLORREF Shape3D::FillColor (void)
{
	return crFill;
}

/*--------------------------------------------------------------*
 * Default constructor.                                         *
 * Entry:  COLORREF (optional) - border color                   *
 *                               default RGB(0,0,0)             *
 *         COLORREF (optional) - fill color                     *
 *                               default RGB(255,255,255)       *
 * Return: COLORREF - border color                              *
 *--------------------------------------------------------------*/
Shape3D::Shape3D (COLORREF border, COLORREF fill)
{
	crBorder = border;
	crFill = fill;
}

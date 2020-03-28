/********************************************************************
 *                                                                  *
 *   Source File: CCR.cpp                                           *
 *                                                                  *
 *   Merely a structure to describe a cubes corner and rotation.    *
 *                                                                  *
 *   by Shaun Miller, 1994                                          *
 *                                                                  *
 ********************************************************************/

#include "CCR.h"

CCR::CCR ()
{
	cube = corner = 0;
	rotation = 3;
}

CCR::CCR (const CCR &cc)
{
	*this = cc;
}

CCR& CCR::operator = (const CCR &cc)
{
	cube = cc.cube;
	corner = cc.corner;
	rotation = cc.rotation;
	return *this;
}

CCR::~CCR ()
{
}

int CCR::operator == (CCR &cc)
{
	return (cube==cc.cube && corner==cc.corner && rotation==cc.rotation);
}

int CCR::operator != (CCR &cc)
{
	return !(*this==cc);
}


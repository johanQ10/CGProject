#include "Point.h"

CPoint::CPoint()
{
	x = y = z = 0;
}

CPoint::~CPoint()
{
}

CPoint::CPoint(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

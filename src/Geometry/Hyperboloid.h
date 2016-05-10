#pragma once
#include "Core/Kaguya.h"
#include "Geometry/Shape.h"

/************************************************************************/
/* Hyperboloid Function Definition                                      */
/************************************************************************/
typedef enum
{
	ONE_SHEET = 1,
	TWO_SHEET = -1,
	CONE = 0
}HYPERBOLOID_TYPE;

class geoHyperboloid :public Shape
{
public:
	geoHyperboloid();
	geoHyperboloid(const Point3f &pos, const Float& semiA, const Float& semiB, const Float& semiC, HYPERBOLOID_TYPE newType);
	~geoHyperboloid();

	void setCenter(const Point3f &pos);
	void setSemiAxes(const Float& semiA, const Float& semiB, const Float& semiC);
	void setHyperboloidType(HYPERBOLOID_TYPE newType);
	bool intersect(const Ray& inRay, DifferentialGeometry* queryPoint, Float *tHit, Float *rayEpsilon) const;
	//Normal3f getNormal(const Point3f &pos) const;

	bool isInside(const Point3f &pPos) const;

public:
	Point3f c;//center
	Float sa, sb, sc;//semi-principal axes of length a, b, c
	HYPERBOLOID_TYPE hbType;
};
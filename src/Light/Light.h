//
//  Light.h
//
//  Created by Shenyao Ke on 2/13/15.
//  Copyright (c) 2015 AKIKA. All rights reserved.
//
#pragma once
#ifndef __Light__
#define __Light__

#include "Core/rtdef.h"
#include "Math/CGVector.h"
#include "Light/Spectrum.h"

typedef enum LIGHT_TYPE
{
	LT_NULL = 0,
	LT_POINT_LIGHT,
	LT_DIRECTIONAL_LIGHT,
	LT_SPOT_LIGHT,
	LT_AREA_LIGHT
};
typedef enum LIGHT_DECAY_TYPE
{
	DECAY_CONSTANT = 0,
	DECAY_QUADRATIC = 1,

};
/************************************************************************/
/* Basic Light                                                          */
/************************************************************************/
class Light
{
protected:
	LIGHT_TYPE type = LT_NULL;
	Spectrum lightSpectrum;
	Float exposure = 0;
	//ColorRGB color = ColorRGB(1.0, 1.0, 1.0);
	LIGHT_DECAY_TYPE decayType = DECAY_CONSTANT;
	Point3D pos;
	Float radius = 0;
public:

	Light();
	Light(const Spectrum& its);
	virtual ~Light();

	virtual void setExposure(const Float& xps);
	virtual void setDecayType(LIGHT_DECAY_TYPE dctype);
	virtual void setRadius(const Float& rd);

	virtual LIGHT_TYPE getLightType() const;
	virtual ColorRGBA getColor() const;
	virtual Float getIntensity(const Float& dist) const;
	virtual Float getIntensity(const DifferentialGeometry *queryPoint) const;
	virtual Spectrum getSpectrum(const DifferentialGeometry *queryPoint) const;
	virtual void getDirection(const DifferentialGeometry *queryPoint) const;
	virtual Float getDistance(const DifferentialGeometry *queryPoint) const;
	virtual void printInfo() const;

	//
	virtual Float getSpecAmout(const Vector3D& DifferentialGeometry, const Vector3D& reflectDir) const;
};
/************************************************************************/
/* Directional Light                                                    */
/************************************************************************/
class directionalLight :public Light
{
	Vector3D dir = Vector3D(0, 0, -1);
public:
	directionalLight();
	directionalLight(const Vector3D& vec);
	directionalLight(const Vector3D& vec, const Spectrum& spt);
	~directionalLight();

	void printInfo() const;
	// 	Vector3D getDirFromPoint(const Vector3D& pointPos) const;
	// 	Float getDistanceFromPoint(const Vector3D& pointPos) const;
	Float getIntensity(const Float& dist) const;
	Float getIntensity(const DifferentialGeometry *queryPoint) const;
	void getDirection(const DifferentialGeometry *queryPoint) const;
	Float getDistance(const DifferentialGeometry *queryPoint) const;
protected:

private:
};
/************************************************************************/
/* Point Light                                                          */
/************************************************************************/
class pointLight :public Light
{
	//Vector3D pos;
public:
	pointLight();
	pointLight(const Vector3D& vec, const Float& its);
	pointLight(const Vector3D& vec, const Spectrum& spt);
	~pointLight();

	void printInfo() const;
protected:

private:

};
/************************************************************************/
/* Spot Light                                                           */
/************************************************************************/
class spotLight :public Light
{
protected:
	//Vector3D pos;
	Vector3D dir;
	Float coneAngle = 40;
	Float penumbraAngle = 0;
	Float cosCA = cos(DegreeToRadian(cosCA));
	Float cosPA = cos(DegreeToRadian(coneAngle + penumbraAngle));
	Float dropoff = 0;
public:
	spotLight();
	spotLight(const Vector3D& pPos, const Vector3D& pDir);
	spotLight(const Vector3D& pPos, const Vector3D& pDir, const Float& ca, const Float& pa, const Float& dpo);
	spotLight(const Vector3D& pPos, const Vector3D& pDir, const Float& ca, const Float& pa, const Float& dpo, const Spectrum& spt);
	~spotLight();

	void printInfo() const;
	void setAngles(const Float& ca, const Float& pa);
	void updateCosAngle();
	void setDropOff(const Float& dpo);

	Float getIntensity(const DifferentialGeometry *queryPoint) const;
	Vector3D getLightPos() const;
};
/************************************************************************/
/* Spot Light                                                           */
/************************************************************************/
class ImageSpotLight :public spotLight
{
	Texture* tex = NULL;
	Vector3D nx, ny;
	Float sx = 1, sy = 1;
public:
	ImageSpotLight();
	ImageSpotLight(const Vector3D& pPos, const Vector3D& pDir, const Vector3D& upVec, const Float& ca, const Float& pa, const Float& dpo, const Spectrum& spt);
	~ImageSpotLight();

	void assignImage(Texture* &newTex);
	void setSize(const Float& xSize, const Float& ySize);
	Spectrum getSpectrum(const DifferentialGeometry *queryPoint) const;
	Float getIntensity(const DifferentialGeometry *queryPoint) const;

protected:

private:
};
/************************************************************************/
/* Area Light                                                           */
/************************************************************************/
typedef enum AREA_LIGHT_SHAPE
{
	QUAD = 0,
	DISK = 1,
	CYLINDER = 2
};
class areaLight :public Light
{
	Vector3D nx, ny, nz;
	//Vector3D pos;
	Float size = 1;//radius
	AREA_LIGHT_SHAPE shapeType = QUAD;
public:
	areaLight();
	areaLight(const Vector3D& pVec, const Float& shpSize);
	areaLight(const Vector3D& pVec, const Float& shpSize, const Spectrum& spt);
	areaLight(const Vector3D& pVec, const Vector3D& dir, const Vector3D& up, const Float& shpSize, const Spectrum& spt);
	~areaLight();

	Float getIntensity(const DifferentialGeometry *queryPoint) const;
	void getDirection(const DifferentialGeometry *queryPoint) const;
	Float getDistance(const DifferentialGeometry *queryPoint) const;
protected:

private:
};
/************************************************************************/
/* Shaped Light                                                         */
/************************************************************************/
/*
typedef enum SPEC_SHAPE
{
SPEC_SHAPE_SPHERE = 0,
SPEC_SHAPE_RECTANGULAR = 1,
SPEC_SHAPE_HEART = 2
};
class shapedLight :public Light
{
Vector3D nx, ny, nz;
//Vector3D pos;
Float shapeSize = 10;
SPEC_SHAPE shapeType = SPEC_SHAPE_SPHERE;
public:
shapedLight(){};
shapedLight(const Vector3D& pVec, const Vector3D& dir, const Vector3D& up, const Float& shpSize, const Spectrum& spt);
~shapedLight(){};

void setShape(SPEC_SHAPE shpType);
Float getSpecAmout(const Vector3D& DifferentialGeometry, const Vector3D& reflectDir) const;
Vector3D getDirFromPoint(const Vector3D& pointPos) const;
protected:

private:
};
shapedLight::shapedLight(const Vector3D& pVec, const Vector3D& dir, const Vector3D& up, const Float& shpSize, const Spectrum& spt)
{
pos = pVec;
nz = dir.getNorm();

nx = nz.crossMul(up).getNorm();
ny = nx.crossMul(nz);

lightSpectrum = spt;
// 	intensity = its;
// 	color = lc;
}
void shapedLight::setShape(SPEC_SHAPE shpType)
{
shapeType = shpType;
}
Float shapedLight::getSpecAmout(const Vector3D& DifferentialGeometry, const Vector3D& reflectDir) const
{
Float x = (DifferentialGeometry - pos)*nx - reflectDir * nx * (nz * (DifferentialGeometry - pos)) / (nz * reflectDir);
Float y = (DifferentialGeometry - pos)*ny - reflectDir * ny * (nz * (DifferentialGeometry - pos)) / (nz * reflectDir);
x /= shapeSize;
y /= shapeSize;
//cout << "x, y:\t" << x << ", " << y << endl;
if (abs(x) < 1 && abs(y) < 1)
{
switch (shapeType)
{
case SPEC_SHAPE_SPHERE:
if (x * x + y * y <= 1)
{
return 1;
}
break;
case SPEC_SHAPE_RECTANGULAR:
return 1;
break;
case SPEC_SHAPE_HEART:
{
Float radius = 0.5;
Float dist = 1.5;
Float result = min(max(abs(x) - radius * cos((y - radius) * PI / dist) - radius, abs(y + radius / 2) - radius * 1.5),
max(min((x + radius) * (x + radius) + (y - radius) * (y - radius) - radius * radius, (x - radius) * (x - radius) + (y - radius) * (y - radius) - radius * radius), -y));
if (result < 0)
{
return 1;
}
break;
}
default:
break;
}
return 0;
}
else
return 0;
}
Vector3D shapedLight::getDirFromPoint(const Vector3D& pointPos) const
{
return (this->pos - pointPos).getNorm();
}*/
#endif
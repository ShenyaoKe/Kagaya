#include "Light/LegacyLight.h"

#include "Geometry/Intersection.h"
#include "Shading/Texture.h"

namespace Kaguya::Legacy
{
/************************************************************************/
/* Basic Light                                                          */
/************************************************************************/
Light::Light()
{
}
Light::Light(const Spectrum &its)
{
	lightSpectrum = its;
}
Light::~Light()
{
}
void Light::setExposure(Float xps)
{
	exposure = xps;
}
void Light::setDecayType(LIGHT_DECAY_TYPE dctype)
{
	decayType = dctype;
}
void Light::setRadius(Float rd)
{
	radius = rd;
}
LIGHT_TYPE Light::getLightType() const
{
	return type;
}
Spectrum Light::getSpectrum(const Intersection* isec) const
{
	//Spectrum ret;// = Spectrum(getIntensity(isec), lightSpectrum.color);
	return Spectrum();
}
Float Light::getDistance(const Intersection* isec) const
{
	return (pos - isec->P).length();
}
void Light::printInfo() const
{
	lightSpectrum.printInfo();
	// 	std::cout << "Intensity:\t" << intensity.intensity << std::endl;
	// 	color.printInfo();
}
/************************************************************************/
/* Directional Light                                                    */
/************************************************************************/
directionalLight::directionalLight()
{
	type = LT_DIRECTIONAL_LIGHT;
}
directionalLight::directionalLight(const Vector3f &vec)
{
	dir = normalize(vec); type = LT_DIRECTIONAL_LIGHT;
}
directionalLight::directionalLight(const Vector3f &vec, const Spectrum &spt)
{
	dir = normalize(vec); lightSpectrum = spt; type = LT_DIRECTIONAL_LIGHT;
}
directionalLight::~directionalLight()
{
}
void directionalLight::printInfo() const
{
	//std::cout << "Intensity:\t" << intensity << std::endl;
	std::cout << "Directional Light Direction:\t" << dir << std::endl;
	lightSpectrum.printInfo();
	//color.printInfo();
}
Float directionalLight::getDistance(const Intersection* isec) const
{
	return INFINITY;
}
/************************************************************************/
/* Point Light                                                          */
/************************************************************************/
pointLight::pointLight()
{
	type = LT_POINT_LIGHT;
}
pointLight::pointLight(const Point3f &p, Float its)
{
	pos = p; lightSpectrum.intensity = its; type = LT_POINT_LIGHT;
}
pointLight::pointLight(const Point3f &p, const Spectrum &spt)
{
	pos = p; lightSpectrum = spt; type = LT_POINT_LIGHT;
}
pointLight::~pointLight()
{
}
void pointLight::printInfo() const
{
	std::cout << "Point Light Position:\t" << pos << std::endl;
	lightSpectrum.printInfo();
}
/************************************************************************/
/* Spot Light                                                           */
/************************************************************************/
spotLight::spotLight()
{
	type = LT_SPOT_LIGHT;
}
spotLight::spotLight(const Point3f &p, const Vector3f &d)
{
	pos = p;
	dir = d;
	type = LT_SPOT_LIGHT;
}
spotLight::~spotLight()
{
}
spotLight::spotLight(const Point3f &p, const Vector3f &d, Float ca, Float pa, Float dpo)
{
	pos = p;
	dir = normalize(d);
	setAngles(ca, pa);
	dropoff = dpo;
	type = LT_SPOT_LIGHT;
}
spotLight::spotLight(const Point3f &p, const Vector3f &d, Float ca, Float pa, Float dpo, const Spectrum &spt)
{
	pos = p;
	dir = normalize(d);
	setAngles(ca, pa);
	dropoff = dpo;
	lightSpectrum = spt;
	type = LT_SPOT_LIGHT;
}
void spotLight::printInfo() const
{

}
void spotLight::setAngles(Float ca, Float pa)
{
	coneAngle = ca;
	penumbraAngle = pa;
	updateCosAngle();
}
void spotLight::updateCosAngle()
{
	cosCA = cos(degreeToRadian(coneAngle));
	cosPA = cos(degreeToRadian(coneAngle + penumbraAngle));
}
void spotLight::setDropOff(Float dpo)
{
	dropoff = dpo;
}
Float spotLight::getIntensity(const Intersection* isec) const
{
	Float dist = getDistance(isec);
	Float tmpIts = dot(isec->P - pos, dir);

	if (penumbraAngle != 0)
	{
		tmpIts = tmpIts > cosCA ? cosCA : (tmpIts < cosPA ? cosPA : tmpIts);
		tmpIts = (tmpIts - cosPA) / (cosCA - cosPA);
		tmpIts = 0.5 - 0.5 * cos(tmpIts * M_PI);
	}
	else
	{
		tmpIts = tmpIts >= cosCA ? 1 : 0;
	}
	if (exposure == 0 && decayType == DECAY_CONSTANT)
	{
		return tmpIts;
	}
	else
	{
		return tmpIts * pow(2, exposure) / (dist * dist);
	}
}
/************************************************************************/
/* Area Light                                                           */
/************************************************************************/
areaLight::areaLight()
{
	nx = Vector3f(1, 0, 0);
	ny = Vector3f(0, 1, 0);
	nz = Vector3f(0, 0, 1);
	type = LT_AREA_LIGHT;
}
areaLight::areaLight(const Point3f &p, Float shpSize)
{
	nx = Vector3f(1, 0, 0);
	ny = Vector3f(0, 1, 0);
	nz = Vector3f(0, 0, 1);
	pos = p;
	size = shpSize;
	type = LT_AREA_LIGHT;
}
areaLight::areaLight(const Point3f &p, Float shpSize, const Spectrum &spt)
{
	nx = Vector3f(1, 0, 0);
	ny = Vector3f(0, 1, 0);
	nz = Vector3f(0, 0, 1);
	pos = p;
	size = shpSize;
	lightSpectrum = spt;
	type = LT_AREA_LIGHT;
}
areaLight::areaLight(const Point3f &p, const Vector3f &dir, const Vector3f &up, Float shpSize, const Spectrum &spt)
{
	nz = normalize(dir);
	nx = normalize(cross(nz, up));
	ny = cross(nx, nz);
	pos = p;
	size = shpSize;
	lightSpectrum = spt;
	type = LT_AREA_LIGHT;
}
areaLight::~areaLight()
{
}
Float areaLight::getIntensity(const Intersection* isec) const
{
	/*if (isec->lightDir * nz > 0)//If on the other side
	{
	return 0;
	}*/
	//
	if (exposure == 0 && decayType == DECAY_CONSTANT)
	{
		return lightSpectrum.intensity;
		;
	}
	else if (decayType == DECAY_CONSTANT)
	{
		return lightSpectrum.intensity * pow(2, exposure);
	}
	else
	{
		Float dist = getDistance(isec);
		return lightSpectrum.intensity * pow(2, exposure) / dist / dist;
	}
}

}

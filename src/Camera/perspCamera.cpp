#include "perspCamera.h"

perspCamera::perspCamera(const Vector3D& eye,
	const Vector3D& targ, const Vector3D& up,
	Float asp, Float lr, Float fd, const Film &fm)
	: Camera(eye, targ, up, asp, lr, fd, fm)
{
	updateMatrices();
}

perspCamera::perspCamera(const Transform& cam2wo, const Transform& projection)
{
	CameraToWorld = cam2wo;
	CameraToScreen = projection;
}

perspCamera::~perspCamera()
{

}

void perspCamera::updateCamToScreen()
{
	Float horisize, vertsize;

	if (viewportRatio - film.ApectureRatio() < 0)
	{
		horisize = film.horiApect;
		vertsize = horisize / viewportRatio;
	}
	else
	{
		vertsize = film.vertApect;
		horisize = vertsize * viewportRatio;
	}

	CameraToScreen.setMat(Matrix4D::PerspectiveFromFilm(
		vertsize, horisize, focLen, nearPlane, farPlane));
}

Float perspCamera::generateRay(const cameraSampler &sample, Ray* ray) const
{
	Vector3D pCam = RasterToCamera(Vector3D(sample.imgX, sample.imgY, 0), 1.);
	*ray = Ray(Vector3D(0, 0, 0), Normalize(pCam));
	// Depth of Field Operations;
	if (lensRadius > 0.)
	{
		//sample point on lens
		Float lensU, lensV;
		//sample lensU and lensV to (-1,1)
		lensU = unitRandom(20) * 2.0 - 1.0;
		lensV = unitRandom(20) * 2.0 - 1.0;
		lensU *= lensRadius;
		lensV *= lensRadius;//scale to focal radius

		//compute point on plane of focus
		Float ft = focalDistance / ray->d.z;
		Point3D focusP = (*ray)(ft);
		//update ray of lens
		ray->o = Vector3D(lensU, lensV, 0);
		ray->d = Normalize(focusP - ray->o);
	}
	CameraToWorld(*ray, ray);
	return 1.0;
}

void perspCamera::renderImg(int x, int y, ColorRGBA& pixColor)
{
	//film.setRGBA(x, y, pixColor);
}

void perspCamera::saveResult(const char* filename)
{
	//film.writeFile(filename);
}

void perspCamera::resizeViewport(Float aspr)
{
	viewportRatio = aspr;
	updateCamToScreen();
	updateRasterToScreen();
}

void perspCamera::setDoF(Float lr, Float fd)
{
	lensRadius = lr;
	focalDistance = fd;
}
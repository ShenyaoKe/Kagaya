#pragma once
#include "Camera/Camera.h"

namespace Kaguya
{

class ProjectiveCamera : public Camera
{
public:
	ProjectiveCamera(const Point3f &eye = Point3f(1, 1, 1),
					 const Point3f &targ = Point3f(0, 0, 0),
					 const Vector3f &up = Vector3f(0, 1, 0),
					 Float aspectRatio = 1, Float lensRadius = 0,
					 Float focalDist = sNumInfinity,
					 const Film &film = Film());
	virtual ~ProjectiveCamera() {}

	void setTarget(const Point3f &targ) { mViewTarget = targ; }
	Point3f getTarget() const { return mViewTarget; }

	void lookAt(const Point3f &eye = Point3f(1, 1, 1),
				const Point3f &targ = Point3f(0, 0, 0),
				const Vector3f &up = Vector3f(0, 1, 0));
	void setProjection(const Matrix4x4 &perspMat);
	void setCamToWorld(const Matrix4x4 &cam2wMat);

	void updateRasterToCam() override;
	void updateCamToScreen() override = 0;
	void updateRasterToScreen() override;

	// Camera Roaming Operation
	void zoom(Float x_val = 0, Float y_val = 0, Float z_val = 0);
	void rotate(Float x_rot = 0, Float y_rot = 0, Float z_rot = 0);
	void rotatePYR(Float pitch = 0, Float yaw = 0, Float roll = 0);
	// Aspect ratio = width / height
	virtual void resizeViewport(Float aspr = 1.0);

	const Float* cam_to_world() const { return CameraToWorld.getMat().data(); }
	const Float* world_to_cam() const { return CameraToWorld.getInvMat().data(); }
	const Float* cam_to_screen() const { return CameraToScreen.getMat().data(); }
	const Float* screen_to_cam() const { return CameraToScreen.getInvMat().data(); }
	const Float* raster_to_screen() const { return RasterToScreen.getMat().data(); }
	const Float* screen_to_raster() const { return RasterToScreen.getInvMat().data(); }

	Transform CameraToWorld;
	Transform CameraToScreen, RasterToCamera, RasterToScreen;

protected:
	Point3f mViewTarget;

	//renderBuffer buffer;

	Float mViewportRatio;//width / height
	Float mFocalLength;//focal length
	Float mLensRadius;
	Float mFocalDistance;

	Float mNearPlane = 0.1f;
	Float mFarPlane = 100;
	uint8_t mSample = 1;

};

}

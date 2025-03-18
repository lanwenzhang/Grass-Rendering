#pragma once

#include "../../glframework/core.h"
#include "camera.h"
#include <map>


class CameraControl {
public:
	CameraControl();
	~CameraControl();

	virtual void onMouse(int button, int action, double xpos, double ypos);
	virtual void onCursor(double xpos, double ypos);
	virtual void onKey(int key, int action, int mods);
	virtual void onScroll(float offset);

	virtual void update();

	void setCamera(Camera* camera) { mCamera = camera;}
	void setSensitivity(float s) { mSensitivity = s;}
	void setScale(float s) { mScaleSpeed = s;}

protected:

	// 1 Mouse button status
	bool mLeftMouseDown = false;
	bool mRightMouseDown = false;
	bool mMiddleMouseDown = false;

	// 2 Mouse position status
	float mCurrentX = 0.0f, mCurrentY = 0.0f;

	// 3 Sensitivity
	float mSensitivity = 0.2f;

	// 4 Keyboard status
	std::map<int, bool> mKeyMap;

	// 5 Camera
	Camera* mCamera = nullptr;

	// 6 Scale speed
	float mScaleSpeed = 0.2;
};
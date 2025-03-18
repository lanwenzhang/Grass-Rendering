#include "cameraControl.h"
#include <iostream>

CameraControl::CameraControl(){}
CameraControl::~CameraControl(){}


void CameraControl::onMouse(int button, int action, double xpos, double ypos){

	std::cout << "onMouse" << std::endl;
;
	// 1 Whether press
	bool pressed = action == GLFW_PRESS ? true : false;


	// 2 Record current position
	if (pressed) {

		mCurrentX = xpos;
		mCurrentY = ypos;
	}

	// 3 Change press status
	switch (button) {
	case GLFW_MOUSE_BUTTON_LEFT:
		mLeftMouseDown = pressed;
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		mRightMouseDown = pressed;
		break;
	case GLFW_MOUSE_BUTTON_MIDDLE:
		mMiddleMouseDown = pressed;
		break;
	}
}

void CameraControl::onCursor(double xpos, double ypos){

	std::cout << "onCursor" << std::endl;
}

void CameraControl::onKey(int key, int action, int mods){


	std::cout << "onKey" << std::endl;
	// Filter repeat
	if (action == GLFW_REPEAT) {
		return;
	}
	// 1 Whether press or release
	bool pressed = action == GLFW_PRESS ? true : false;

	// 2 Record
	mKeyMap[key] = pressed;
}

void CameraControl::update(){

}

void CameraControl::onScroll(float offset){}

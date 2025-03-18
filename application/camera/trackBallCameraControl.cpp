#include "trackBallCameraControl.h"

TrackBallCameraControl::TrackBallCameraControl(){}
TrackBallCameraControl::~TrackBallCameraControl(){}

void TrackBallCameraControl::onCursor(double xpos, double ypos){
	if (mLeftMouseDown) {

		// 1 Calculate the x and y position change
		float deltaX = (xpos - mCurrentX) * mSensitivity;
		float deltaY = (ypos - mCurrentY) * mSensitivity;
		
		// 2 Calculate pitch
		pitch(deltaY);
		yaw(deltaX);
	}
	else if (mMiddleMouseDown) {
		float deltaX = (xpos - mCurrentX) * mMoveSpeed;
		float deltaY = (ypos - mCurrentY) * mMoveSpeed;

		mCamera->mPosition += mCamera->mUp * deltaY;
		mCamera->mPosition -= mCamera->mRight * deltaX;
	}

	mCurrentX = xpos;
	mCurrentY = ypos;
}

void TrackBallCameraControl::pitch(float angle){

	// 1 Rotate along the right vector
	auto mat = glm::rotate(glm::mat4(1.0f), glm::radians(angle), mCamera->mRight);

	// 2 Update the camera 
	mCamera->mUp = mat * glm::vec4(mCamera->mUp, 0.0f);
	mCamera->mPosition = mat * glm::vec4(mCamera->mPosition, 1.0f);
}

void TrackBallCameraControl::yaw(float angle) {

	auto mat = glm::rotate(glm::mat4(1.0f), glm::radians(angle),glm::vec3(0.0f, 1.0f, 0.0f));

	mCamera->mUp = mat * glm::vec4(mCamera->mUp, 0.0f);
	mCamera->mRight = mat * glm::vec4(mCamera->mRight, 0.0f);
	mCamera->mPosition = mat * glm::vec4(mCamera->mPosition, 1.0f);
}

void TrackBallCameraControl::onScroll(float offset) {

	mCamera->scale(mScaleSpeed * offset);
}
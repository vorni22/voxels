#include "camera.h"

// Public

Camera::Camera(float _screenWidth, float _screenHeight, float _fov, float _pitch, float _yaw, glm::vec3 _position, glm::vec3 _worldUp, float _nearPlane, float _farPlane){
	fov = _fov;
	Pitch = _pitch;
	Yaw = _yaw;
	Position = _position;
	WorldUp = _worldUp;
	ScreenHeight = _screenHeight;
	ScreenWidth = _screenWidth;
	nearPlane = _nearPlane;
	farPlane = _farPlane;
	needToUpdatePerspectice = true;
	updateCameraVectors();
}

glm::vec3 Camera::cameraPos(){
	return Position;
}

glm::vec3 Camera::cameraFront(){
	return Front;
}

glm::vec3 Camera::cameraRight(){
	return Right;
}

glm::vec3 Camera::cameraUp(){
	return Up;
}

glm::mat4 Camera::viewMatrix(){
	return glm::lookAt(Position, Position + Front, Up);
}

glm::mat4 Camera::perspectiveMatrix(){
	return glm::perspective(glm::radians(fov), ScreenWidth / ScreenHeight, nearPlane, farPlane);
	needToUpdatePerspectice = false;
}

glm::vec2 Camera::getScreenSize(){
	return glm::vec2(ScreenWidth, ScreenHeight);
}

void Camera::setScreenSize(float _width, float _height){
	ScreenWidth = _width;
	ScreenHeight = _height;
	needToUpdatePerspectice = true;
}

float Camera::getFOV(){
	return fov;
}

void Camera::setFOV(float _fov){
	fov = _fov;
	needToUpdatePerspectice = true;
}

bool Camera::NeedToGetNewPerspective(){
	return needToUpdatePerspectice;
}

float Camera::getYaw(){
	return YAW;
}

float Camera::getPitch(){
	return PITCH;
}

void Camera::cameraSetFOV(float _fov){
	fov = _fov;
}

void Camera::cameraRotate(float _delta_yaw, float _delta_pitch, bool constrainPitch, float constrainVal){
	Pitch += _delta_pitch;
	Yaw += _delta_yaw;

	if (constrainPitch) {
		constrainVal = abs(constrainVal);
		if (Pitch > constrainVal)
			Pitch = constrainVal;
		if (Pitch < -constrainVal)
			Pitch = -constrainVal;
	}

	updateCameraVectors();
}

void Camera::cameraMove(float CameraFront, float CameraUp, float CameraRight){
	Position += CameraFront * Front + CameraUp * Up + CameraRight * Right;
}

void Camera::cameraSetPos(glm::vec3 _pos){
	Position = _pos;
}

void Camera::moveUp(float amount){
	Position.y += amount;
}


// Private

void Camera::updateCameraVectors() {
	// calculate the new Front vector
	Front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	Front.y = sin(glm::radians(Pitch));
	Front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	Front = glm::normalize(Front);
	// also re-calculate the Right and Up vector
	Right = glm::normalize(glm::cross(Front, WorldUp)); 
	Up = glm::normalize(glm::cross(Right, Front));
}

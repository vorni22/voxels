#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float FOV = 45.0f;
const float NEAR = 0.1f;
const float FAR = 500.0f;

class Camera {
public:
	Camera(float _screenWidth, float _screenHeight,
		   float _fov = FOV, float _pitch = PITCH, float _yaw = YAW,
		   glm::vec3 _position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 _worldUp = glm::vec3(0.0f, 1.0f, 0.0f), 
		   float _nearPlane = NEAR, float _farPlane = FAR);

	// Accesors
	glm::vec3 cameraPos();

	glm::vec3 cameraFront();
	glm::vec3 cameraRight();
	glm::vec3 cameraUp();
	
	glm::mat4 viewMatrix();
	glm::mat4 perspectiveMatrix();

	float getYaw();
	float getPitch();

	// perspective
	glm::vec2 getScreenSize();
	void setScreenSize(float _width, float _height);
	float getFOV();
	void setFOV(float _fov);
	bool NeedToGetNewPerspective();


	// camera movement
	void cameraSetFOV(float _fov = FOV);
	void cameraRotate(float _delta_yaw, float _delta_pitch, bool constrainPitch = true, float constrainVal = 89.0f);
	void cameraMove(float CameraFront, float CameraUp, float CameraRight);
	void cameraSetPos(glm::vec3 _pos);
	void moveUp(float amount);

private:
	// camera atributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;

	// perspective
	float fov;
	float ScreenWidth, ScreenHeight;
	float nearPlane, farPlane;

	// euler Angles
	float Yaw;
	float Pitch;

	bool needToUpdatePerspectice;

	void updateCameraVectors();
};

#endif
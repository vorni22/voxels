#include "mouse.h"

double Mouse::x = 0, Mouse::y = 0;

double Mouse::lastX = 0, Mouse::lastY = 0;

double Mouse::dx = 0, Mouse::dy = 0;

double Mouse::scrollDx = 0, Mouse::scrollDy = 0;

bool Mouse::firstMouse = true;

bool Mouse::buttons[GLFW_MOUSE_BUTTON_LAST] = { 0 };
bool Mouse::buttonsChanged[GLFW_MOUSE_BUTTON_LAST] = { 0 };

bool Mouse::EnforceCursor = false;
bool Mouse::sw = false;

void Mouse::cursorPosCallback(GLFWwindow* window, double _x, double _y){
	x = _x;
	y = _y;

	if (firstMouse) {
		lastX = x;
		lastY = y;
		firstMouse = false;
	}

	dx = x - lastX;
	dy = lastY - y;

	lastX = x;
	lastY = y;

	if (EnforceCursor && sw) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		sw = false;
	}

	if (!EnforceCursor && sw) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		sw = false;
	}
}

void Mouse::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods){
	// buttons[button] = action != GLFW_RELEASE;
	// buttonsChanged[button] = action != GLFW_REPEAT;
	// GLFW_REPEAT does not work for the mouse buttons

	if (action == GLFW_RELEASE) {
		if (buttons[button]) buttonsChanged[button] = true;
		buttons[button] = false;
	}

	if (action == GLFW_PRESS) {
		if (!buttons[button]) buttonsChanged[button] = true;
		buttons[button] = true;
	}
}

void Mouse::mouseWheelCallback(GLFWwindow* window, double dx, double dy){
	scrollDx = dx;
	scrollDy = dy;
}

double Mouse::getMouseX() { return x;  }

double Mouse::getMouseY() { return y; }

double Mouse::getMouseDX() { 
	double _dx = dx;
	dx = 0;
	return _dx;
}

double Mouse::getMouseDY() { 
	double _dy = dy;
	dy = 0;
	return _dy;
}

double Mouse::getMouseScrollDX(){
	double _sdx = scrollDx;
	scrollDx = 0;
	return _sdx;
}

double Mouse::getMouseScrollDY(){
	double _sdy = scrollDy;
	scrollDy = 0;
	return _sdy;
}

bool Mouse::button(int button){
	return buttons[button];
}

bool Mouse::buttonChanged(int button){
	bool ret = buttonsChanged[button];
	buttonsChanged[button] = false;
	return ret;
}

bool Mouse::buttonWentUp(int button){
	return !buttons[button] && buttonChanged(button);
}

bool Mouse::buttonWentDown(int button)
{
	return buttons[button] && buttonChanged(button);
}

void Mouse::centerCursor(bool val) {
	EnforceCursor = val;
	sw = true;
}

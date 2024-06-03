#ifndef MOUSE_H
#define MOUSE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Mouse {
public:
	// Callbacks
	static void cursorPosCallback(GLFWwindow* window, double _x, double _y);
	static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void mouseWheelCallback(GLFWwindow* window, double dx, double dy);

	// Accesors
	static double getMouseX();
	static double getMouseY();

	static double getMouseDX();
	static double getMouseDY();

	static double getMouseScrollDX();
	static double getMouseScrollDY();

	static bool button(int button);
	static bool buttonChanged(int button);
	static bool buttonWentUp(int button);
	static bool buttonWentDown(int button);

	static void centerCursor(bool val);

private:
	static double x, y;

	static double lastX, lastY;

	static double dx, dy;

	static double scrollDx, scrollDy;
	
	static bool firstMouse;

	static bool buttons[];
	static bool buttonsChanged[];

	static bool EnforceCursor;

	static bool sw;
};

#endif
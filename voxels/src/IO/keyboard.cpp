#include "keyboard.h"

bool Keyboard::keys[GLFW_KEY_LAST] = { 0 };
bool Keyboard::keysChanged[GLFW_KEY_LAST] = { 0 };

void Keyboard::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods){
	// keys[key] = action != GLFW_RELEASE;
	// keysChanged[key] = action != GLFW_REPEAT; 
	// GLFW_REPEAT does not mean that a key's state is repeated
	// A maximum of only 1 key can be repeated at a time!

	if (action == GLFW_RELEASE) {
		if (keys[key]) keysChanged[key] = true;
		keys[key] = false;
	}

	if (action == GLFW_PRESS) {
		if (!keys[key]) keysChanged[key] = true;
		keys[key] = true;
	}
}

// Accesors

bool Keyboard::key(int key){
	return keys[key];
}

bool Keyboard::keyChanged(int key){
	bool ret = keysChanged[key];
	keysChanged[key] = false;
	return ret;
}

bool Keyboard::keyWentUp(int key){
	return !keys[key] && keyChanged(key); 
}

bool Keyboard::keyWentDown(int key){
	return keys[key] && keyChanged(key);
}

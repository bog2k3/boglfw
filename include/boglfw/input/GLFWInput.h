/*
 * GLFWInput.h
 *
 *  Created on: Nov 7, 2014
 *      Author: bog
 */

#ifndef INPUT_GLFWINPUT_H_
#define INPUT_GLFWINPUT_H_

#ifdef WITH_GLFW

#include "InputEvent.h"
#include "../utils/Event.h"
#include <functional>
#include <glm/vec2.hpp>
#include <vector>

class GLFWwindow;

class GLFWInput {
public:
	static void initialize(GLFWwindow* window);

	// Returns true if application should continue, and false if it should shut down (user closed window)
	static bool checkInput();
	// Resets the input event queue by discarding any pending events.
	// This is usually done before the first frame, to ensure you don't treat any events that occured during window initialization.
	static void resetInputQueue();

	static Event<void(InputEvent&)> onInputEvent;

private:
	static void glfwMouseScroll(GLFWwindow* win,double x, double y);
	static void glfwMouseButton(GLFWwindow* win, int button, int action, int mods);
	static void glfwMousePos(GLFWwindow* win, double x, double y);
	static void glfwKey(GLFWwindow* win, int key, int scancode, int action, int mods);
	static void glfwChar(GLFWwindow* win, unsigned int ch);

	static InputEvent::MOUSE_BUTTON translateMouseButton(int glfwBtn);

	static std::vector<InputEvent> eventQueue;
	static GLFWwindow *window;
	static glm::vec2 lastMousePos;
};

#else
#error "GLFW support not enabled in this build, use -DWITH_GLFW to enable it."
#endif // WITH_GLFW

#endif /* INPUT_GLFWINPUT_H_ */

/*
 * GLFWInput.cpp
 *
 *  Created on: Nov 7, 2014
 *      Author: bog
 */

#ifdef WITH_GLFW

#include <boglfw/input/GLFWInput.h>
#include <boglfw/input/InputEvent.h>
#include <boglfw/utils/log.h>

#include <GLFW/glfw3.h>

#ifdef DEBUG_DMALLOC
#include <dmalloc.h>
#endif

GLFWwindow* GLFWInput::window = nullptr;
std::vector<InputEvent> GLFWInput::eventQueue;
glm::vec2 GLFWInput::lastMousePos;
Event<void(InputEvent&)> GLFWInput::onInputEvent;

void GLFWInput::initialize(GLFWwindow* pWindow) {
	window = pWindow;
	glfwSetScrollCallback(window, &GLFWInput::glfwMouseScroll);
	glfwSetMouseButtonCallback(window, &GLFWInput::glfwMouseButton);
	glfwSetCursorPosCallback(window, &GLFWInput::glfwMousePos);
	glfwSetKeyCallback(window, &GLFWInput::glfwKey);
	glfwSetCharCallback(window, &GLFWInput::glfwChar);
}

bool GLFWInput::checkInput() {
	glfwPollEvents();
	for (InputEvent& ev : eventQueue) {
		onInputEvent.trigger(std::ref(ev));
	}
	eventQueue.clear();
	return glfwWindowShouldClose(window) == 0;
}

void GLFWInput::resetInputQueue() {
	glfwPollEvents();
	eventQueue.clear();
}

InputEvent::MOUSE_BUTTON GLFWInput::translateMouseButton(int glfwBtn) {
	switch (glfwBtn) {
		case GLFW_MOUSE_BUTTON_LEFT:
			return InputEvent::MB_LEFT;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			return InputEvent::MB_MIDDLE;
		case GLFW_MOUSE_BUTTON_RIGHT:
			return InputEvent::MB_RIGHT;
		default:
			return InputEvent::MB_NONE;
	}
}

void GLFWInput::glfwMouseScroll(GLFWwindow* win, double x, double y) {
	eventQueue.push_back(InputEvent(InputEvent::EV_MOUSE_SCROLL, lastMousePos.x, lastMousePos.y, 0, 0, y, InputEvent::MB_NONE, 0, 0));
}
void GLFWInput::glfwMouseButton(GLFWwindow* win, int button, int action, int mods) {
	InputEvent::EVENT_TYPE evType = action == GLFW_PRESS ? InputEvent::EV_MOUSE_DOWN : InputEvent::EV_MOUSE_UP;
	auto btn = translateMouseButton(button);
	eventQueue.push_back(InputEvent(evType, lastMousePos.x, lastMousePos.y, 0, 0, 0, btn, 0, 0));
}
void GLFWInput::glfwMousePos(GLFWwindow* win, double x, double y) {
	glm::vec2 delta = glm::vec2(x, y) - lastMousePos;
	lastMousePos = glm::vec2(x, y);
	eventQueue.push_back(InputEvent(InputEvent::EV_MOUSE_MOVED, x, y, delta.x, delta.y, 0, InputEvent::MB_NONE, 0, 0));
}
void GLFWInput::glfwKey(GLFWwindow* win, int key, int scancode, int action, int mods) {
	if (action == GLFW_REPEAT)
		return;
	InputEvent::EVENT_TYPE evType = action == GLFW_PRESS ? InputEvent::EV_KEY_DOWN : InputEvent::EV_KEY_UP;
	eventQueue.push_back(InputEvent(evType, 0, 0, 0, 0, 0, InputEvent::MB_NONE, key, 0));
}

void GLFWInput::glfwChar(GLFWwindow* win, unsigned int ch) {
	eventQueue.push_back(InputEvent(InputEvent::EV_KEY_CHAR, 0, 0, 0, 0, 0, InputEvent::MB_NONE, 0, (char)ch));
}

#endif // WITH_GLFW

#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Core/Camera.h>

#include <Config/LayoutConfig.h>

namespace LandscapeEngine {
	class InputConfig {
	public:
		static GLFWwindow *contextWindow;

	public:
		static void setActiveCamera(Camera* camera) {
			activeCamera = camera;
			glfwSetInputMode(contextWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

			firstMouse = true;
		}

		static void unsetActiveCamera() {
			glfwSetInputMode(contextWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

			activeCamera = nullptr;
		}

		static bool getMouseButtonDown(int button) {
			if (glfwGetMouseButton(contextWindow, button) == GLFW_PRESS) {
				if (!hasPressed) {
					hasPressed = true;
					return true;
				}
			}

			return false;
		}

		static void processInput(float frameTime) {
			if (hasPressed) {
				if (glfwGetMouseButton(InputConfig::contextWindow, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
					hasPressed = false;
				}
			}

			if (activeCamera == nullptr) {
				return;
			}

			if (glfwGetKey(contextWindow, GLFW_KEY_W) == GLFW_PRESS) {
				activeCamera->translate(FORWARD, frameTime);
			}
			if (glfwGetKey(contextWindow, GLFW_KEY_S) == GLFW_PRESS) {
				activeCamera->translate(BACKWARD, frameTime);
			}
			if (glfwGetKey(contextWindow, GLFW_KEY_A) == GLFW_PRESS) {
				activeCamera->translate(LEFT, frameTime);
			}
			if (glfwGetKey(contextWindow, GLFW_KEY_D) == GLFW_PRESS) {
				activeCamera->translate(RIGHT, frameTime);
			}
		}

		// executes whenever the window size changed
		static void screenSizeCallback(GLFWwindow* window, int width, int height) {
			// make sure the viewport matches the new window dimensions;
			glViewport(0, 0, width, height);

			// change the size of screenwindow
			LayoutConfig::setScreenWindowSize(width, height);
		}

		static void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
			if (activeCamera == nullptr) {
				return;
			}

			if (firstMouse) {
				lastX = xpos;
				lastY = ypos;
				firstMouse = false;
			}

			// reversed since y-coordinates go from bottom to top
			float xoffset = xpos - lastX;
			float yoffset = lastY - ypos;

			lastX = xpos;
			lastY = ypos;

			activeCamera->processMouseMovement(xoffset, yoffset);
		}


		static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
			if (activeCamera == nullptr) {
				return;
			}

			activeCamera->processMouseScroll(yoffset);
		}
	private:
		static Camera* activeCamera;

		static bool firstMouse;
		static float lastX, lastY;

		static bool hasPressed;
	};

	GLFWwindow* InputConfig::contextWindow = nullptr;

	Camera* InputConfig::activeCamera = nullptr;
	float InputConfig::lastX = 0.0f;
	float InputConfig::lastY = 0.0f;
	bool InputConfig::firstMouse = true;
	bool InputConfig::hasPressed = false;
}
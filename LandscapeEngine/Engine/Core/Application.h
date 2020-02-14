#pragma once

/* should include glad before glfw */
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>

#include <Config/LayoutConfig.h>
#include <Config/InputConfig.h>
#include <Config/OpenGLConfig.h>

#include <Editor/Editor.h>

#include <Utility/Log.h>

#include <iostream>

namespace LandscapeEngine {
	class Application {
	public:
		// frame time
		float frameTime;

		float framePerSecond;

	public:
		Application(unsigned int width, unsigned int height, const char* name = "Default", const char* icon = NULL) {
			/* set the size of screen */
			LayoutConfig::setScreenWindowSize(width, height);

			/* initialize log */
			logInit();

			/* initialize glfw */
			glfwInit();

			/* create window */
			_window = glfwCreateWindow(width, height, name, NULL, NULL);
			if (_window == NULL) {
				log("Failed to create GLFW window");
				glfwTerminate();
				return;
			}

			/* create a window icon */
			if (icon != NULL) {
				GLFWimage images[1];
				//rgba channels
				images[0].pixels = stbi_load("Resources/Textures/Icon/window_icon.bmp", &images[0].width, &images[0].height, 0, 4);
				glfwSetWindowIcon(_window, 1, images);
				stbi_image_free(images[0].pixels);
			}

			/* set current window */
			glfwMakeContextCurrent(_window);
			/* config glfw */
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OpenGLConfig::MajorVersion);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OpenGLConfig::MinorVersion);
			glfwWindowHint(GLFW_OPENGL_PROFILE, OpenGLConfig::Profile);
#ifdef __APPLE__
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

			/* GLAD: load all OpenGL function pointers */
			if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
				log("Failed to initialize GLAD");
			}

			/* set context window */
			InputConfig::contextWindow = _window;

			/* create Editor after load Opengl function pointers */
			_editor = new Editor();
		}

		void loop() {
			/* set up callback function */
			glfwSetFramebufferSizeCallback(InputConfig::contextWindow, InputConfig::screenSizeCallback);
			glfwSetCursorPosCallback(InputConfig::contextWindow, InputConfig::mouse_callback);
			glfwSetScrollCallback(InputConfig::contextWindow, InputConfig::scroll_callback);

			while (isRunning()) {
				/* process input */
				InputConfig::processInput(frameTime);

				_startTime = glfwGetTime();

				// draw editor
				this->_editor->draw();

				swapBuffersAndPollEvents();

				_endTime = glfwGetTime();
				frameTime = _endTime - _startTime;

				_averageTimer += frameTime;
				if (_averageTimer >= 1.0f) {
					framePerSecond = 1.0f / frameTime;
					char title[128];
					sprintf_s(title, "Landscape Engine -- Frame time: %.2f ms, FPS: %.0f", frameTime * 1000, 1.0 / frameTime);
					glfwSetWindowTitle(_window, title);
					_averageTimer = 0;
				}
			}

			terminate();
		}

	private:
		GLFWwindow *_window;
		Editor *_editor;

		// time variables indicating the start and end of the frame
		float _startTime, _endTime;
		float _averageTimer;

	private:
		bool isRunning() {
			if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
				glfwSetWindowShouldClose(_window, true);
			}

			return !glfwWindowShouldClose(_window);
		}

		void swapBuffersAndPollEvents() {
			// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
			glfwSwapBuffers(this->_window);
			glfwPollEvents();
		}

		void terminate() {
			// shut down editor
			this->_editor->shutdown();

			// terminate glfw, clearing all previously allocated GLFW resources.
			glfwTerminate();
		}
	};
}
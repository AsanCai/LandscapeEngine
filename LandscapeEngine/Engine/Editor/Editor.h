#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext/vector_uint2.hpp>

#include <Config/LayoutConfig.h>
#include <Config/InputConfig.h>

#include <Editor/ToolWindow.h>
#include <Editor/TextureWindow.h>
#include <Editor/ScreenshotWindow.h>

#include <Core/Framebuffer.h>
#include <Core/Shader.h>

#include <Landscape/Landscape.h>

#include <Utility/Log.h>
#include <Utility/Texture.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_glad.h>

// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

namespace LandscapeEngine {
	class Editor {
	public:
		Editor() {
			// setup Dear ImGui context
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO();
			// avoid creating imgui.ini
			io.IniFilename = NULL;
			// enable Keyboard Controls
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
			// enable Gamepad Controls
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

			// setup Dear ImGui style
			ImGui::StyleColorsClassic();

			/* setup Platform/Renderer bindings */
			const char* glsl_version = "#version 460 core";
			ImGui_ImplGlfw_InitForOpenGL(InputConfig::contextWindow, true);
			ImGui_ImplOpenGL3_Init(glsl_version);

			_landscape = new Landscape(*LayoutConfig::SceneWindowSize);

			/* create window */
			_sceneWindow = new TextureWindow(
				LayoutConfig::SceneWindowSize,
				LayoutConfig::SceneWindowPos,
				_landscape->getSceneTexture(),
				"Scene",
				ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar
				| ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoResize
			);
			_sceneWindow->bindCamera(_landscape->camera);

			_screenShotWindow = new ScreenshotWindow(
				LayoutConfig::ScreenShotWindowSize,
				LayoutConfig::ScreenShotWindowPos,
				_landscape->getSceneTexture(),
				"ScreenShot",
				ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize
				| ImGuiWindowFlags_HorizontalScrollbar
			);

			_toolWindow = new ToolWindow(
				LayoutConfig::ToolWindowSize,
				LayoutConfig::ToolWindowPos,
				"Tool",
				ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse
				| ImGuiWindowFlags_NoResize
			);
			_toolWindow->bindLandscape(_landscape);
			_toolWindow->bindVolumetricCloud(_landscape->getVolumetricCloud());
		}


		void draw() {
			/* make sure to draw landscape before editor */
			drawLandscape();

			drawEditor();
		}

		void shutdown() {
			// clean up
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();
		}

	private:
		/* draw landscape */
		void drawLandscape() {
			_landscape->draw();
		}

		/* draw editor */
		void drawEditor() {
			// bind framebuffer
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			// start the Dear ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			/* draw the tool window */
			_toolWindow->draw();

			/* draw the scene window */
			_sceneWindow->draw();

			/* draw the menu window */
			_screenShotWindow->draw();

			// Rendering
			ImGui::Render();
			glClear(GL_COLOR_BUFFER_BIT);
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

	private:
		Landscape *_landscape;

		ToolWindow *_toolWindow;
		TextureWindow *_sceneWindow;
		ScreenshotWindow *_screenShotWindow;
	};
}
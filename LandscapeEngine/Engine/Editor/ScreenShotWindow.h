#pragma once

#include <Editor/BaseWindow.h>

#include <Config/LayoutConfig.h>

#include <Core/Framebuffer.h>
#include <Core/Shader.h>

#include <Utility/ScreenQuad.h>
#include <Utility/Texture.h>

#include <vector>

namespace LandscapeEngine{
	class ScreenshotWindow : public BaseWindow {
	public:
		ScreenshotWindow(glm::vec2* size, glm::vec2* pos, unsigned int screen, const std::string name = "ScreenShot", ImGuiWindowFlags flags = 0)
			: BaseWindow(size, pos, name, flags) {
			// set the value of the property
			_screenTexture = screen;
			_screenshotShader = new Shader("Shaders/ScreenQuad.vert", "Shaders/ScreenShot.frag");
			_screenshotList = new std::vector<unsigned int>();

			// register event createScreenShot
			Dispatcher::getInstance()->registerEvent(new Event<>("createScreenShot", &ScreenshotWindow::createScreenshot));
			// register event exportScreenShots
			Dispatcher::getInstance()->registerEvent(new Event<>("exportScreenShots", &ScreenshotWindow::exportScreenshots));
		}

		static std::vector<unsigned int>* getScreenShots() {
			return _screenshotList;
		}

		virtual void draw() {
			// set window padding to zero
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(1.0f, 1.0f));

			/* draw the scene window */
			ImGui::SetNextWindowPos(ImVec2(_position->x, _position->y));
			ImGui::SetNextWindowSize(ImVec2(_size->x, _size->y));

			ImGui::Begin(_name.c_str(), NULL, _windowFlags);

			// get the mouse position
			ImVec2 pos = ImGui::GetCursorScreenPos();

			for (std::vector<unsigned int>::iterator iter = _screenshotList->begin(); iter != _screenshotList->end(); iter++) {
				ImGui::Image(
					(void*)(intptr_t)(*iter),
					ImVec2(this->_size->y * LayoutConfig::SceneRatio, this->_size->y),
					ImVec2(0, 0), ImVec2(1, 1)
				);
				ImGui::SameLine();
			}

			ImGui::End();

			// pop style var
			ImGui::PopStyleVar();
		}

	private:
		static unsigned int _screenTexture;
		static Shader *_screenshotShader;
		static std::vector<unsigned int> *_screenshotList;

	private:
		// create screenshot
		static void createScreenshot() {
			Framebuffer frame(*LayoutConfig::SceneWindowSize);
			frame.bind();

			_screenshotShader->use();
			_screenshotShader->setSampler2D("screen", _screenTexture, 0);
			renderQuad();

			_screenshotList->push_back(frame.texture);
			frame.unbind();
		}

		// export screenshots
		static void exportScreenshots() {
			// the directory to save all screen shots
			std::string directory = "ScreenShots/";
			std::string path;
			int index = 0;
			for (std::vector<unsigned int>::iterator iter = _screenshotList->begin(); iter != _screenshotList->end(); iter++) {
				index++;
				path = directory + std::to_string(index) + ".png";
				saveTextureToPNG((*iter), path);
			}
		}
	};

	unsigned int ScreenshotWindow::_screenTexture = 0;
	Shader *ScreenshotWindow::_screenshotShader = nullptr;
	std::vector<unsigned int> *ScreenshotWindow::_screenshotList = new std::vector<unsigned int>();
}
#pragma once

#include <Core/Camera.h>

#include <Editor/BaseWindow.h>

namespace LandscapeEngine{
	class TextureWindow : public BaseWindow {
	public:
		TextureWindow(glm::vec2* size, glm::vec2* pos, unsigned int texture, const std::string name = "DefaultWindow", ImGuiWindowFlags flags = 0)
			: BaseWindow(size, pos, name, flags) {
			// set the value of the property
			_texture = texture;
			_windowCamera = nullptr;
		}

		virtual void draw() {
			// set window padding to zero
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(1.0f, 1.0f));

			/* draw the scene window */
			ImGui::SetNextWindowPos(ImVec2(this->_position->x, this->_position->y));
			ImGui::SetNextWindowSize(ImVec2(this->_size->x, this->_size->y));

			ImGui::Begin(_name.c_str(), NULL, _windowFlags);
			if (_windowCamera != nullptr) {
				if (_active) {
					if (InputConfig::getMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT)) {
						InputConfig::unsetActiveCamera();
						_active = false;
					}
				}
				else {
					if (ImGui::IsWindowHovered() && InputConfig::getMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT)) {
						InputConfig::setActiveCamera(_windowCamera);
						_active = true;
					}
				}
			}

			// get the mouse position
			ImVec2 pos = ImGui::GetCursorScreenPos();
			// render image
			ImGui::GetWindowDrawList()->AddImage(
				(void *)_texture,
				pos, ImVec2(pos.x + _size->x, pos.y + _size->y),
				ImVec2(0, 1), ImVec2(1, 0)
			);

			// we are done working with this window
			ImGui::End();

			// pop style var
			ImGui::PopStyleVar();
		}

		void bindCamera(Camera* camera) {
			_windowCamera = camera;
		}

	private:
		unsigned int _texture;
		Camera *_windowCamera;
		bool _active = false;
	};
}
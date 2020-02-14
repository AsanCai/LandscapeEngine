#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_glad.h>

#include <Event/Dispatcher.h>

#include <string>

namespace LandscapeEngine {
	class BaseWindow {
	public:
		BaseWindow(glm::vec2* size, glm::vec2* pos, const std::string name = "Default", ImGuiWindowFlags flags = 0) {
			_size = size;
			_position = pos;
			_name = name;
			_windowFlags = flags;
		}


		virtual void draw() = 0;

	protected:
		glm::vec2 *_size;
		glm::vec2 *_position;
		std::string _name;
		ImGuiWindowFlags _windowFlags;
	};
}
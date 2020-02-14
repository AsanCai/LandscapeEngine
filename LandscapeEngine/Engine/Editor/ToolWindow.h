#pragma once

#include <Config/LayoutConfig.h>

#include <Landscape/Landscape.h>
#include <Landscape/VolumetricCloud.h>

#include <Editor/BaseWindow.h>

#include <Utility/ScreenQuad.h>

#include <vector>

namespace LandscapeEngine{
	class ToolWindow : public BaseWindow {
	public:
		ToolWindow(glm::vec2* size, glm::vec2* pos, const std::string name = "Tool", ImGuiWindowFlags flags = 0)
			: BaseWindow(size, pos, name, flags) {
			// set the value of the property
			_landscape = nullptr;
			_cloud = nullptr;
		}

		virtual void draw() {
			// set window padding to zero
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(1.0f, 1.0f));
			/* draw the menu window */
			ImGui::SetNextWindowPos(ImVec2(LayoutConfig::ToolWindowPos->x, LayoutConfig::ToolWindowPos->y));
			ImGui::SetNextWindowSize(ImVec2(LayoutConfig::ToolWindowSize->x, LayoutConfig::ToolWindowSize->y));

			ImGui::Begin(this->_name.c_str(), NULL, this->_windowFlags);

			// draw Landscape widget
			if (_landscape != nullptr) {
				ImGui::Text("Landscape widget");
				ImGui::SliderFloat("Sun Rise Angle", &_landscape->sunRiseAngle, 0.0f, 180.0f);
				ImGui::SliderFloat("Sun Rotate Angle", &_landscape->sunRotateAngle, 0.0f, 360.0f);
				ImGui::Separator();
			}

			// draw Volumetric Cloud widget
			if (_cloud != nullptr) {
				ImGui::Text("Volumetric Cloud widget");
				ImGui::Checkbox("Optimization", &_cloud->optimization);
				ImGui::Checkbox("Bayer Filter", &_cloud->bayerFilter);
				ImGui::InputInt("Sample Point", &_cloud->samplePoint, 32, 32);
				ImGui::SliderFloat("Cloud Coverage", &_cloud->coverage, 0.0f, 1.0f);
				ImGui::SliderFloat("Cloud Type", &_cloud->cloudType, 0.0f, 1.0f);
				ImGui::SliderFloat("Cloud Speed", &_cloud->cloudSpeed, 0.0f, 500.0f);
				ImGui::ColorEdit3("Cloud Color", &_cloud->cloudColor[0]);
				ImGui::Separator();
			}

			// draw Screenshot widget
			ImGui::Text("Screenshot widget");
			if (ImGui::Button("Create Screenshot")) {
				// diapatch event createScreenShot
				Dispatcher::getInstance()->dispatchEvent("createScreenShot");
			}
			ImGui::SameLine();
			if (ImGui::Button("Export Screenshots")) {
				// diapatch event exportScreenShots
				Dispatcher::getInstance()->dispatchEvent("exportScreenShots");
			}
			ImGui::Separator();

			ImGui::End();
			// pop style var
			ImGui::PopStyleVar();
		}

		void bindLandscape(Landscape* landscape) {
			_landscape = landscape;
		}

		void bindVolumetricCloud(VolumetricCloud* cloud) {
			_cloud = cloud;
		}

	private:
		VolumetricCloud *_cloud;
		Landscape *_landscape;
	};
}
#pragma once

#include <glm/glm.hpp>
#include <glm/ext/vector_uint2.hpp>

namespace LandscapeEngine {
	class LayoutConfig {
	public:
		/* Make sure not to change the value of these variables directly */
		static glm::vec2* ScreenWindowSize;
		static glm::vec2* SceneWindowSize;
		static glm::vec2* ToolWindowSize;
		static glm::vec2* ScreenShotWindowSize;

		static glm::vec2* SceneWindowPos;
		static glm::vec2* ToolWindowPos;
		static glm::vec2* ScreenShotWindowPos;

		static float SceneRatio;
		static glm::vec2 ScreenScaler;

		static void setScreenWindowSize(unsigned int width, unsigned int height) {
			ScreenScaler.x = (float)width / (float)ScreenWindowSize->x;
			ScreenScaler.y = (float)height / (float)ScreenWindowSize->y;

			/* Just change the value of pointer */
			*SceneWindowSize = ScreenScaler * (*SceneWindowSize);
			*ScreenWindowSize = glm::vec2(width, height);
			*ToolWindowSize = glm::vec2(
				ScreenWindowSize->x - SceneWindowSize->x,
				ScreenWindowSize->y
			);
			*ScreenShotWindowSize = glm::vec2(
				SceneWindowSize->x,
				ScreenWindowSize->y - SceneWindowSize->y
			);


			*ToolWindowPos = glm::vec2(0, 0);
			*SceneWindowPos = glm::vec2(ToolWindowSize->x, 0);
			*ScreenShotWindowPos = glm::vec2(ToolWindowSize->x, SceneWindowSize->y);

			SceneRatio = (float)SceneWindowSize->x / (float)SceneWindowSize->y;
		}
	};

	glm::vec2* LayoutConfig::ScreenWindowSize = new glm::vec2(1280, 720);
	glm::vec2* LayoutConfig::SceneWindowSize = new glm::vec2(880, 500);
	glm::vec2* LayoutConfig::ToolWindowSize = new glm::vec2(
		LayoutConfig::ScreenWindowSize->x - LayoutConfig::SceneWindowSize->x,
		LayoutConfig::ScreenWindowSize->y
	);
	glm::vec2* LayoutConfig::ScreenShotWindowSize = new glm::vec2(
		LayoutConfig::SceneWindowSize->x,
		LayoutConfig::ScreenWindowSize->y - LayoutConfig::SceneWindowSize->y
	);

	glm::vec2* LayoutConfig::ToolWindowPos = new glm::vec2(0, 0);
	glm::vec2* LayoutConfig::SceneWindowPos = new glm::vec2(LayoutConfig::ToolWindowSize->x, 0);
	glm::vec2* LayoutConfig::ScreenShotWindowPos = new glm::vec2(
		LayoutConfig::ToolWindowSize->x,
		LayoutConfig::SceneWindowSize->y
	);

	float LayoutConfig::SceneRatio = (float)LayoutConfig::SceneWindowSize->x / (float)LayoutConfig::SceneWindowSize->y;
	glm::vec2 LayoutConfig::ScreenScaler = glm::vec2(1.0f);
}
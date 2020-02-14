#pragma once

#include <GLFW/glfw3.h>

namespace LandscapeEngine {
	class OpenGLConfig {
	public:
		static unsigned int MajorVersion;
		static unsigned int MinorVersion;
		static unsigned int Profile;
	};

	unsigned int OpenGLConfig::MajorVersion = 4;
	unsigned int OpenGLConfig::MinorVersion = 6;
	unsigned int OpenGLConfig::Profile = GLFW_OPENGL_CORE_PROFILE;
}
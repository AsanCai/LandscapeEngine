#include <Core/Application.h>

using namespace LandscapeEngine;

// main function
int main(int argc, char* argv[]) {
	OpenGLConfig::MajorVersion = 4;
	OpenGLConfig::MinorVersion = 6;

	Application app(1280, 720, "Landscape Engine", "Resources/Textures/Icon/window_icon.bmp");
	
	app.loop();

	return 0;
}
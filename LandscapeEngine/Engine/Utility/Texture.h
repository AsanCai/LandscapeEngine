/* Texture from learnopengl */

#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace LandscapeEngine {
	struct Texture {
		GLuint id;
		// used in load material texture to draw model
		std::string type;
		// used in load material texture to draw model
		std::string path;
	};

	// create 2D texture from image
	void texture2DFromFile(Texture* texture, const char *path, const std::string &directory, std::string type = "");


	unsigned int texture3DFromCFT3(const char* path);
	unsigned int loadCubemap(std::vector<std::string> faces);
	unsigned int createTexture2D(int w, int h);
	unsigned int createTexture3D(int w, int h, int d);

	void saveTextureToPNG(unsigned int texture, std::string path);
}
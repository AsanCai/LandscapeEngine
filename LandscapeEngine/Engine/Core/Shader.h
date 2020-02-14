#pragma once

#include <Core/BaseShader.h>

namespace LandscapeEngine {
	class Shader : public BaseShader {
	public:
		// constructor generates the shader and attach them to the program
		Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr) {
			// create shader
			unsigned vertex, fragment, geometry;
			vertex = createShader(vertexPath, GL_VERTEX_SHADER);
			fragment = createShader(fragmentPath, GL_FRAGMENT_SHADER);
			if (geometryPath != nullptr) {
				geometry = createShader(geometryPath, GL_GEOMETRY_SHADER);
			}

			// create program and attach shader
			_id = glCreateProgram();
			glAttachShader(_id, vertex);
			glAttachShader(_id, fragment);
			if (geometryPath != nullptr) {
				glAttachShader(_id, geometry);
			}

			// link and compile program
			glLinkProgram(_id);
			checkCompileErrors(_id, "PROGRAM");

			// delete shader
			glDeleteShader(vertex);
			glDeleteShader(fragment);
			if (geometryPath != nullptr) {
				glDeleteShader(geometry);
			}
		}

		// get shader's type
		ShaderType getShaderType() {
			return SHADER;
		}
	};
}
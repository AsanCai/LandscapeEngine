#pragma once

#include <Core/BaseShader.h>

namespace LandscapeEngine {
	class TessellationShader : public BaseShader {
	public:
		TessellationShader(const char* vertexPath, const char* fragmentPath, const char* tessControlPath, const char* tessEvalPath) {
			// create shader
			unsigned vertex, fragment, tessControl, tessEvaluation;
			vertex = createShader(vertexPath, GL_VERTEX_SHADER);
			fragment = createShader(fragmentPath, GL_FRAGMENT_SHADER);
			tessControl = createShader(tessControlPath, GL_TESS_CONTROL_SHADER);
			tessEvaluation = createShader(tessEvalPath, GL_TESS_EVALUATION_SHADER);

			// create program and attach shader
			_id = glCreateProgram();
			glAttachShader(_id, vertex);
			glAttachShader(_id, fragment);
			glAttachShader(_id, tessControl);
			glAttachShader(_id, tessEvaluation);

			// link and compile program
			glLinkProgram(_id);
			checkCompileErrors(_id, "PROGRAM");

			// delete shader
			glDeleteShader(vertex);
			glDeleteShader(fragment);
			glDeleteShader(tessControl);
			glDeleteShader(tessEvaluation);
		}

		// get shader's type
		ShaderType getShaderType() {
			return TESSELLATION_SHADER;
		}
	};
}
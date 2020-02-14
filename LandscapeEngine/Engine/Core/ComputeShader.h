#pragma once

#include <Core/BaseShader.h>

namespace LandscapeEngine{
	class ComputeShader : public BaseShader {
	public:
		ComputeShader(const char* computePath) {
			// create shader
			unsigned int compute;
			compute = createShader(computePath, GL_COMPUTE_SHADER);

			// create program and attach shader
			_id = glCreateProgram();
			glAttachShader(_id, compute);

			// link and compile program
			glLinkProgram(_id);
			checkCompileErrors(_id, "PROGRAM");

			// delete shader
			glDeleteShader(compute);
		}

		// get shader's type
		ShaderType getShaderType() {
			return COMPUTE_SHADER;
		}
	};
}
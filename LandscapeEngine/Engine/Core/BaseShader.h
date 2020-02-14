#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Utility/Log.h>
#include <Utility/Texture.h>

#include <string>
#include <fstream>
#include <sstream>

namespace LandscapeEngine {
	// define shader type
	enum ShaderType {
		SHADER,
		COMPUTE_SHADER,
		TESSELLATION_SHADER
	};

	class BaseShader {
	public:
		// get shader's type
		virtual ShaderType getShaderType() = 0;

		// activate the shader
		void use() {
			glUseProgram(_id);
		}

		// utility uniform functions
		void setBool(const std::string &name, bool value) const {
			glUniform1i(glGetUniformLocation(_id, name.c_str()), (int)value);
		}

		void setInt(const std::string &name, int value) const {
			glUniform1i(glGetUniformLocation(_id, name.c_str()), value);
		}

		void setFloat(const std::string &name, float value) const {
			glUniform1f(glGetUniformLocation(_id, name.c_str()), value);
		}

		void setVec2(const std::string &name, const glm::vec2 &value) const {
			glUniform2fv(glGetUniformLocation(_id, name.c_str()), 1, &value[0]);
		}

		void setVec2(const std::string &name, float x, float y) const {
			glUniform2f(glGetUniformLocation(_id, name.c_str()), x, y);
		}

		void setVec3(const std::string &name, const glm::vec3 &value) const {
			glUniform3fv(glGetUniformLocation(_id, name.c_str()), 1, &value[0]);
		}

		void setVec3(const std::string &name, float x, float y, float z) const {
			glUniform3f(glGetUniformLocation(_id, name.c_str()), x, y, z);
		}

		void setVec4(const std::string &name, const glm::vec4 &value) const {
			glUniform4fv(glGetUniformLocation(_id, name.c_str()), 1, &value[0]);
		}

		void setVec4(const std::string &name, float x, float y, float z, float w) {
			glUniform4f(glGetUniformLocation(_id, name.c_str()), x, y, z, w);
		}

		void setMat2(const std::string &name, const glm::mat2 &mat) const {
			glUniformMatrix2fv(glGetUniformLocation(_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
		}

		void setMat3(const std::string &name, const glm::mat3 &mat) const {
			glUniformMatrix3fv(glGetUniformLocation(_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
		}

		void setMat4(const std::string &name, const glm::mat4 &mat) const {
			glUniformMatrix4fv(glGetUniformLocation(_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
		}

		void setSampler1D(const std::string &name, const Texture texture, const GLuint index) {
			this->setInt(name, index);
			glActiveTexture(GL_TEXTURE0 + index);
			glBindTexture(GL_TEXTURE_1D, texture.id);
		}

		void setSampler1D(const std::string &name, const GLuint texture, const GLuint index) {
			this->setInt(name, index);
			glActiveTexture(GL_TEXTURE0 + index);
			glBindTexture(GL_TEXTURE_1D, texture);
		}

		void setSampler2D(const std::string &name, const Texture texture, const GLuint index) {
			this->setInt(name, index);
			glActiveTexture(GL_TEXTURE0 + index);
			glBindTexture(GL_TEXTURE_2D, texture.id);
		}

		void setSampler2D(const std::string &name, const GLuint texture, const GLuint index) {
			this->setInt(name, index);
			glActiveTexture(GL_TEXTURE0 + index);
			glBindTexture(GL_TEXTURE_2D, texture);
		}

		void setSampler3D(const std::string &name, const Texture texture, const GLuint index) {
			this->setInt(name, index);
			glActiveTexture(GL_TEXTURE0 + index);
			glBindTexture(GL_TEXTURE_3D, texture.id);
		}

		void setSampler3D(const std::string &name, const GLuint texture, const GLuint index) {
			this->setInt(name, index);
			glActiveTexture(GL_TEXTURE0 + index);
			glBindTexture(GL_TEXTURE_3D, texture);
		}

	protected:
		// the id of shader program
		GLuint _id;

	protected:
		// utility function for checking shader compilation/linking errors.
		void checkCompileErrors(GLuint shader, std::string type, std::string shaderName = "") {
			GLint success;
			GLchar infoLog[1024];
			if (type != "PROGRAM") {
				glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
				if (success != GL_TRUE) {
					glGetShaderInfoLog(shader, 1024, NULL, infoLog);

					log("ERROR::SHADER: %s COMPILATION ERROR of type : %s \n %s \n", shaderName.c_str(), type.c_str(), infoLog);
				}
			}
			else {
				glGetProgramiv(shader, GL_LINK_STATUS, &success);
				if (success != GL_TRUE) {
					glGetProgramInfoLog(shader, 1024, NULL, infoLog);

					log("ERROR::PROGRAM_LINKING_ERROR of type: %s \n %s \n", type.c_str(), infoLog);

				}
			}
		}

		int createShader(const char* shaderPath, GLenum shaderType) {
			// 1. retrieve the vertex/fragment source code from filePath
			std::string shaderString = loadShaderFromFile(shaderPath);
			const char* shaderCode = shaderString.c_str();

			// 2. compile shaders
			unsigned shaderID;
			shaderID = glCreateShader(shaderType);
			glShaderSource(shaderID, 1, &shaderCode, NULL);
			glCompileShader(shaderID);

			std::string type;
			switch (shaderType) {
			case GL_VERTEX_SHADER:
				type = "VERTEX";
				break;
			case GL_FRAGMENT_SHADER:
				type = "FRAGMENT";
				break;
			case GL_GEOMETRY_SHADER:
				type = "GEOMETRY";
				break;
			case GL_COMPUTE_SHADER:
				type = "COMPUTE";
				break;
			case GL_TESS_CONTROL_SHADER:
				type = "TESSELLATION_CONTROL";
				break;
			case GL_TESS_EVALUATION_SHADER:
				type = "TESSELLATION_EVALUATION";
				break;
			default:
				type = "";
				break;
			}

			// 3. check compile result
			checkCompileErrors(shaderID, type, getShaderName(shaderPath));

			// 4. return shader ID
			return shaderID;
		}

		// utility function for loading shader
		std::string loadShaderFromFile(const char* shaderPath) {
			std::string shaderCode;
			std::ifstream shaderFile;
			shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			try {
				// open files
				shaderFile.open(shaderPath);
				std::stringstream shaderStream;
				// read file's buffer contents into streams
				shaderStream << shaderFile.rdbuf();
				// close file handlers
				shaderFile.close();
				// convert stream into string
				shaderCode = shaderStream.str();
			}
			catch (std::ifstream::failure e) {
				log("ERROR::SHADER %s FILE_NOT_SUCCESFULLY_READ \n", getShaderName(shaderPath).c_str());
			}
			return shaderCode;
		}

		// utility function for getting the name of shader
		std::string getShaderName(const char* path) {
			std::string pathstr = std::string(path);
			const size_t last_slash_idx = pathstr.find_last_of("/");
			if (std::string::npos != last_slash_idx) {
				pathstr.erase(0, last_slash_idx + 1);
			}
			return pathstr;
		}
	};
}
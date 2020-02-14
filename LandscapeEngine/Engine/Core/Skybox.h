#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Core/Shader.h>
#include <Utility/Texture.h>

#include <vector>

namespace LandscapeEngine {
	class Skybox {
	public:
		Skybox() {
			_shader = new Shader("Shaders/Skybox.vert", "Shaders/Skybox.frag");
			float skyboxVertices[] = {
				// positions          
				-1.0f,  1.0f, -1.0f,
				-1.0f, -1.0f, -1.0f,
				1.0f, -1.0f, -1.0f,
				1.0f, -1.0f, -1.0f,
				1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,

				-1.0f, -1.0f,  1.0f,
				-1.0f, -1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f,  1.0f,
				-1.0f, -1.0f,  1.0f,

				1.0f, -1.0f, -1.0f,
				1.0f, -1.0f,  1.0f,
				1.0f,  1.0f,  1.0f,
				1.0f,  1.0f,  1.0f,
				1.0f,  1.0f, -1.0f,
				1.0f, -1.0f, -1.0f,

				-1.0f, -1.0f,  1.0f,
				-1.0f,  1.0f,  1.0f,
				1.0f,  1.0f,  1.0f,
				1.0f,  1.0f,  1.0f,
				1.0f, -1.0f,  1.0f,
				-1.0f, -1.0f,  1.0f,

				-1.0f,  1.0f, -1.0f,
				1.0f,  1.0f, -1.0f,
				1.0f,  1.0f,  1.0f,
				1.0f,  1.0f,  1.0f,
				-1.0f,  1.0f,  1.0f,
				-1.0f,  1.0f, -1.0f,

				-1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f,  1.0f,
				1.0f, -1.0f, -1.0f,
				1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f,  1.0f,
				1.0f, -1.0f,  1.0f
			};

			glGenVertexArrays(1, &_skyboxVertexArrayObject);
			glGenBuffers(1, &skyboxVBO);
			glBindVertexArray(_skyboxVertexArrayObject);
			glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

			_model = glm::translate(_model, glm::vec3(0.0, -0.1, 0.0));

			// load skybox texture
			std::vector<std::string> faces{
				"Resources/textures/skybox/right.jpg",
				"Resources/textures/skybox/left.jpg",
				"Resources/textures/skybox/top.jpg",
				"Resources/textures/skybox/bottom.jpg",
				"Resources/textures/skybox/front.jpg",
				"Resources/textures/skybox/back.jpg"
			};
			_texture = loadCubemap(faces);

			_shader->setInt("skybox", 0);
		}

		Skybox(const char* vertexPath, const char* fragmentPath) {
			_shader = new Shader(vertexPath, fragmentPath);
			float skyboxVertices[] = {
				// positions          
				-1.0f,  1.0f, -1.0f,
				-1.0f, -1.0f, -1.0f,
				1.0f, -1.0f, -1.0f,
				1.0f, -1.0f, -1.0f,
				1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,

				-1.0f, -1.0f,  1.0f,
				-1.0f, -1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f,  1.0f,
				-1.0f, -1.0f,  1.0f,

				1.0f, -1.0f, -1.0f,
				1.0f, -1.0f,  1.0f,
				1.0f,  1.0f,  1.0f,
				1.0f,  1.0f,  1.0f,
				1.0f,  1.0f, -1.0f,
				1.0f, -1.0f, -1.0f,

				-1.0f, -1.0f,  1.0f,
				-1.0f,  1.0f,  1.0f,
				1.0f,  1.0f,  1.0f,
				1.0f,  1.0f,  1.0f,
				1.0f, -1.0f,  1.0f,
				-1.0f, -1.0f,  1.0f,

				-1.0f,  1.0f, -1.0f,
				1.0f,  1.0f, -1.0f,
				1.0f,  1.0f,  1.0f,
				1.0f,  1.0f,  1.0f,
				-1.0f,  1.0f,  1.0f,
				-1.0f,  1.0f, -1.0f,

				-1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f,  1.0f,
				1.0f, -1.0f, -1.0f,
				1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f,  1.0f,
				1.0f, -1.0f,  1.0f
			};

			glGenVertexArrays(1, &_skyboxVertexArrayObject);
			glGenBuffers(1, &skyboxVBO);
			glBindVertexArray(_skyboxVertexArrayObject);
			glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

			_model = glm::translate(_model, glm::vec3(0.0, -0.1, 0.0));

			// load skybox texture
			std::vector<std::string> faces{
				"Resources/textures/skybox/right.jpg",
				"Resources/textures/skybox/left.jpg",
				"Resources/textures/skybox/top.jpg",
				"Resources/textures/skybox/bottom.jpg",
				"Resources/textures/skybox/front.jpg",
				"Resources/textures/skybox/back.jpg"
			};
			_texture = loadCubemap(faces);

			_shader->setInt("skybox", 0);
		}

		~Skybox() {}

		void draw(glm::mat4 view, glm::mat4 proj) {
			glDepthFunc(GL_LEQUAL);

			// remove translation part from the view matrix
			glm::mat4 view2 = glm::mat4(glm::mat3(view));
			_shader->setMat4("view", view2);
			_shader->setMat4("projection", proj);

			// bing VAO and draw skybox
			glBindVertexArray(_skyboxVertexArrayObject);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, _texture);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);

			glDepthMask(GL_TRUE);
			glDepthFunc(GL_LESS);
		}

	private:
		unsigned int _skyboxVertexArrayObject, skyboxVBO;
		unsigned int _texture;
		glm::mat4 _model;
		Shader *_shader;
	};
}
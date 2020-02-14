#pragma once

#include <glad/glad.h>

#include <Core/Framebuffer.h>
#include <Core/Shader.h>
#include <Core/Camera.h>

#include <Landscape/VolumetricCloud.h>

namespace LandscapeEngine{
	class Landscape {
	public:
		float sunRiseAngle;
		float sunRotateAngle;
		Camera *camera;

	public:
		Landscape(glm::vec2 size) : Landscape(size.x, size.y) {}

		Landscape(unsigned int width, unsigned int height) {
			this->camera = new Camera(width, height, glm::vec3(0.0f, 0.0f, 3.0f));
			this->sunRiseAngle = 60.0;
			this->sunRotateAngle = 45.0f;

			_width = width;
			_height = height;

			_fogColor = glm::vec3(0.6 + 0.1, 0.71 + 0.1, 0.85 + 0.1);
			_fogColor *= 0.7;

			_postShader = new Shader("Shaders/ScreenQuad.vert", "Shaders/PostProcessing.frag");
			_landscapeFBO = new Framebuffer(width, height);
			_sceneFBO = new Framebuffer(width, height);
			_volumetricCloud = new VolumetricCloud(width, height, this->camera);
		}

		void draw() {
			float riseRadians = glm::radians(this->sunRiseAngle);
			float rotateRadians = glm::radians(this->sunRotateAngle);
			glm::vec3 lightPosition = 1e9f * glm::vec3(
				glm::cos(riseRadians) * glm::sin(rotateRadians),
				glm::sin(riseRadians),
				glm::cos(riseRadians) * glm::cos(rotateRadians)
			);
			lightPosition += camera->position;

			// view/projection transformations
			glm::mat4 projection = camera->getProjectionMatrix();
			glm::mat4 view = camera->getViewMatrix();

			_landscapeFBO->bind();

			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);

			glClearColor(this->_fogColor[0], this->_fogColor[1], this->_fogColor[2], 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			disableTests();

			_volumetricCloud->draw(view, projection, lightPosition, _landscapeFBO->depthTex);

			// blend volumetric clouds rendering with terrain and apply some post process
			_sceneFBO->bind();

			_postShader->use();
			_postShader->setVec2("resolution", glm::vec2(this->_width, this->_height));
			_postShader->setSampler2D("screenTexture", _landscapeFBO->texture, 0);
			_postShader->setSampler2D("cloudTEX", _volumetricCloud->getCloudsTexture(), 1);
			renderQuad();
		}

		unsigned int getSceneTexture() {
			return _sceneFBO->texture;
		}

		VolumetricCloud* getVolumetricCloud() {
			return _volumetricCloud;
		}

	private:
		unsigned int _width, _height;

		VolumetricCloud *_volumetricCloud;
		Shader *_postShader;
		Framebuffer *_landscapeFBO;
		Framebuffer *_sceneFBO;

		glm::vec3 _fogColor;
	};
}
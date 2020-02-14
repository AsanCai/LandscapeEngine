#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Core/Camera.h>
#include <Core/Shader.h>
#include <Core/ComputeShader.h>
#include <Core/Framebuffer.h>

#include <Utility/Texture.h>
#include <Utility/ScreenQuad.h>

namespace LandscapeEngine{
	class VolumetricCloud {
	public:
		float coverage;
		float cloudType;
		float cloudSpeed;
		glm::vec3 cloudColor;
		int samplePoint;
		bool optimization;
		bool bayerFilter;

		unsigned int weatherTex;

	public:
		VolumetricCloud(int SW, int SH, Camera * cam) : _screenWidth(SW), SCR_HEIGHT(SH), _camera(cam) {
			_volumetricCloudShader = new Shader("Shaders/ScreenQuad.vert", "Shaders/VolumetricCloud.frag");
			_cloudPostprocessingShader = new Shader("Shaders/ScreenQuad.vert", "Shaders/CloudPost.frag");
			_copyShader = new Shader("Shaders/ScreenQuad.vert", "Shaders/CopyFrame.frag");

			_volumetricCloudFramebuffer = new Framebuffer(SW, SH, 3);
			_cloudsPostProcessingFramebuffer = new Framebuffer(SW, SH);
			_lastFramebuffer = new Framebuffer(SH, SH, 2);

			this->coverage = 0.45;
			this->cloudType = 0.1;
			this->cloudSpeed = 500.0;
			this->cloudColor = glm::vec3(162.0 / 255, 177.0 / 255, 206.0 / 255);
			this->samplePoint = 128;
			this->optimization = true;
			this->bayerFilter = true;

			_baseVolumeNoiseTexture = texture3DFromCFT3("Resources/Textures/Noise/base.cft3");
			_erodeVolumeNoise = texture3DFromCFT3("Resources/Textures/Noise/erode.cft3");

			//compute shaders
			ComputeShader weather("Shaders/Weather.comp");
			//make texture
			this->weatherTex = createTexture2D(1024, 1024);
			//compute
			weather.use();
			glDispatchCompute((GLuint)1024, (GLuint)1024, (GLuint)1);

			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		}

		~VolumetricCloud() {}

		void draw(glm::mat4 view, glm::mat4 proj, glm::vec3 sunPosition, unsigned int depthMapTex) {
			// refresh when samplePoint changes
			_refresh = (_preSamplePoint != this->samplePoint)
				|| (_preBayerFilter != this->bayerFilter)
				|| (_preOptimization != this->optimization);

			samplePoint = glm::clamp(this->samplePoint, 32, 256);

			float t1, t2;
			float time = glfwGetTime();

			_volumetricCloudFramebuffer->bind();

			_volumetricCloudShader->use();
			_volumetricCloudShader->setFloat("u_cloudType", this->cloudType);
			_volumetricCloudShader->setFloat("u_coverage", this->coverage);
			_volumetricCloudShader->setFloat("u_cloudSpeed", this->cloudSpeed);
			_volumetricCloudShader->setVec3("u_cloudColor", this->cloudColor);
			_volumetricCloudShader->setVec3("u_cameraPosition", _camera->position);
			_volumetricCloudShader->setVec3("u_sunPosition", sunPosition);
			_volumetricCloudShader->setInt("u_samplePoint", this->samplePoint);
			_volumetricCloudShader->setBool("u_optimization", this->optimization);
			_volumetricCloudShader->setBool("u_refresh", this->_refresh);
			_volumetricCloudShader->setBool("u_bayerFilter", this->bayerFilter);

			_volumetricCloudShader->setVec2("iResolution", glm::vec2(_screenWidth, SCR_HEIGHT));
			_volumetricCloudShader->setFloat("iTime", time);
			_volumetricCloudShader->setMat4("inv_proj", glm::inverse(proj));
			_volumetricCloudShader->setMat4("inv_view", glm::inverse(_camera->getViewMatrix()));
			_volumetricCloudShader->setFloat("FOV", _camera->fov);
			_volumetricCloudShader->setInt("frameIter", _frameIter);

			_volumetricCloudShader->setMat4("invViewProj", glm::inverse(proj*view));
			_volumetricCloudShader->setMat4("oldFrameVP", _laseFrameViewProjectMatrix);
			_volumetricCloudShader->setMat4("gVP", proj * view);

			_volumetricCloudShader->setSampler3D("baseVolumeNoise", this->_baseVolumeNoiseTexture, 0);
			_volumetricCloudShader->setSampler3D("erodeVolumeNoise", this->_erodeVolumeNoise, 1);
			_volumetricCloudShader->setSampler2D("weatherTex", this->weatherTex, 2);
			_volumetricCloudShader->setSampler2D("depthMap", depthMapTex, 3);
			_volumetricCloudShader->setSampler2D("lastFrameAlphaness", _lastFramebuffer->getColorAttachmentTex(0), 4);
			_volumetricCloudShader->setSampler2D("lastFrameColor", _lastFramebuffer->getColorAttachmentTex(1), 5);

			//actual draw
			renderQuad();

			// cloud post processing filtering
			_cloudsPostProcessingFramebuffer->bind();
			_cloudPostprocessingShader->use();

			_cloudPostprocessingShader->setSampler2D("clouds", _volumetricCloudFramebuffer->getColorAttachmentTex(0), 0);
			_cloudPostprocessingShader->setSampler2D("depthMap", depthMapTex, 1);
			_cloudPostprocessingShader->setVec2("resolution", glm::vec2(_screenWidth, SCR_HEIGHT));
			renderQuad();

			//Copy last frame
			_lastFramebuffer->bind();

			_copyShader->use();
			_copyShader->setSampler2D("alphanessTex", _volumetricCloudFramebuffer->getColorAttachmentTex(1), 0);
			_copyShader->setSampler2D("colorTex", _volumetricCloudFramebuffer->getColorAttachmentTex(2), 1);
			renderQuad();

			//copy last VP matrix
			_laseFrameViewProjectMatrix = proj * view;

			//increment frame counter mod 16, for temporal reprojection
			_frameIter = (_frameIter + 1) % 16;

			// update variables
			_preSamplePoint = this->samplePoint;
			_preOptimization = this->optimization;
			_preBayerFilter = this->bayerFilter;
		}

		unsigned int getCloudsTexture() {
			return _cloudsPostProcessingFramebuffer->texture;
		}


	private:
		int _screenWidth, SCR_HEIGHT;
		Camera *_camera;
		Shader *_volumetricCloudShader, *_cloudPostprocessingShader, *_copyShader;

		int _frameIter = 0;
		bool _refresh = false;
		int _preSamplePoint = 0;
		bool _preOptimization = false;
		bool _preBayerFilter = false;
		Framebuffer *_volumetricCloudFramebuffer, *_cloudsPostProcessingFramebuffer, *_lastFramebuffer;

		unsigned int _baseVolumeNoiseTexture, _erodeVolumeNoise;

		glm::mat4 _laseFrameViewProjectMatrix;
	};
}

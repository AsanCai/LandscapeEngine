#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <Utility/Log.h>

namespace LandscapeEngine {
	class Framebuffer {
	public:
		unsigned int depthTex;
		unsigned int texture;

	public:
		Framebuffer(glm::vec2 size) : Framebuffer(size.x, size.y) {}

		Framebuffer(int weight, int height) {
			_width = weight;
			_height = height;
			_id = createFrameBuffer();
			_colorAttachments = NULL;
			_nColorAttachments = 1;

			this->texture = createTextureAttachment(_width, _height);
			this->depthTex = createDepthTextureAttachment(_width, _height);
		}

		Framebuffer(glm::vec2 size, const int nColorAttachments) : Framebuffer(size.x, size.y, nColorAttachments) {}

		Framebuffer(int weight, int height, const int nColorAttachments) {
			_width = weight;
			_height = height;
			_id = createFrameBuffer();
			_colorAttachments = createColorAttachments(_width, _height, nColorAttachments);
			_nColorAttachments = nColorAttachments;

			this->texture = NULL;
			this->depthTex = createDepthTextureAttachment(_width, _height);
			
			unsigned int * colorAttachmentsFlag = new unsigned int[nColorAttachments];
			for (unsigned int i = 0; i < nColorAttachments; i++) {
				colorAttachmentsFlag[i] = GL_COLOR_ATTACHMENT0 + i;
			}
			glDrawBuffers(nColorAttachments, colorAttachmentsFlag);
			delete colorAttachmentsFlag;
		}


		unsigned int getColorAttachmentTex(int i) {
			if (i < 0 || i > _nColorAttachments) {
				log("COLOR ATTACHMENT OUT OF RANGE");
				return 0;
			}
			return _colorAttachments[i];
		}

		void bind() {
			bindFrameBuffer(this->_id, this->_width, this->_height);
		}

		void unbind() {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	private:
		unsigned int _id;
		int _width, _height;
		int _nColorAttachments;
		unsigned int *_colorAttachments;

	private:
		void bindFrameBuffer(int frameBuffer, int width, int height) {
			// To make sure the texture isn't bound
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

			// To make sure the size of viewport
			glViewport(0, 0, width, height);
		}

		void unbindCurrentFrameBuffer(int scrWidth, int scrHeight) {//call to switch to default frame buffer
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, scrWidth, scrHeight);
		}

		unsigned int createFrameBuffer() {
			unsigned int frameBuffer;
			glGenFramebuffers(1, &frameBuffer);
			glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
			glDrawBuffer(GL_COLOR_ATTACHMENT0);

			return frameBuffer;
		}

		unsigned int createTextureAttachment(int width, int height) {
			unsigned int texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);

			return texture;
		}

		unsigned int * createColorAttachments(int width, int height, unsigned int nColorAttachments) {
			unsigned int * colorAttachments = new unsigned int[nColorAttachments];
			glGenTextures(nColorAttachments, colorAttachments);

			for (unsigned int i = 0; i < nColorAttachments; i++) {
				glBindTexture(GL_TEXTURE_2D, colorAttachments[i]);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
				glGenerateMipmap(GL_TEXTURE_2D);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

				glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, colorAttachments[i], 0);
				//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorAttachments[i], 0);
			}
			return colorAttachments;
		}

		unsigned int createDepthTextureAttachment(int width, int height) {
			unsigned int texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture, 0);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture, 0);
			return texture;
		}

		unsigned int createDepthBufferAttachment(int width, int height) {
			unsigned int depthBuffer;
			glGenRenderbuffers(1, &depthBuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
			return depthBuffer;
		}

		unsigned int createRenderBufferAttachment(int width, int height) {
			unsigned int rbo;
			glGenRenderbuffers(1, &rbo);
			glBindRenderbuffer(GL_RENDERBUFFER, rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height); // use a single renderbuffer object for both a depth AND stencil buffer.
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it

			return rbo;
		}
	};
}
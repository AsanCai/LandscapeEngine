#include <glad/glad.h>
#include <Utility/ScreenQuad.h>

namespace LandscapeEngine{
	static GLuint quadVAO = 0;
	static GLuint quadVBO = 0;
	static bool initialized = false;

	void renderQuad() {
		if (!initialized) {
			GLfloat quadVertices[] = {
				// Positions        // Texture Coords
				-1.0f, 1.0f, 0.0f,	0.0f, 1.0f,
				-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				1.0f, 1.0f, 0.0f,	1.0f, 1.0f,
				1.0f, -1.0f, 0.0f,	1.0f, 0.0f,
			};
			glGenVertexArrays(1, &quadVAO);
			glGenBuffers(1, &quadVBO);
			glBindVertexArray(quadVAO);
			glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

			initialized = true;
		}

		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	void disableTests() {
		glDisable(GL_CLIP_DISTANCE0);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
	}
}
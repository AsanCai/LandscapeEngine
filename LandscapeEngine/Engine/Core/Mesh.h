#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Core/Shader.h>
#include <Utility/Texture.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

namespace LandscapeEngine {
	struct Vertex {
		// position
		glm::vec3 Position;
		// normal
		glm::vec3 Normal;
		// texCoords
		glm::vec2 TexCoords;
		// tangent
		glm::vec3 Tangent;
		// bitangent
		glm::vec3 Bitangent;
	};

	class Mesh {
	public:
		/*  Mesh Data  */
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;
		unsigned int vertexArrayObject;

	public:
		// constructor
		Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) {
			this->vertices = vertices;
			this->indices = indices;
			this->textures = textures;

			// now that we have all the required data, set the vertex buffers and its attribute pointers.
			setupMesh();
		}

		// render the mesh
		void draw(Shader shader) {
			// bind appropriate textures
			unsigned int diffuseNr = 1;
			unsigned int specularNr = 1;
			unsigned int normalNr = 1;
			unsigned int heightNr = 1;
			for (unsigned int i = 0; i < textures.size(); i++) {
				// retrieve texture number (the N in diffuse_textureN)
				std::string number;
				//std::string name = textures[i].type;
				std::string name = textures[i].type;
				if (name == "texture_diffuse")
					number = std::to_string(diffuseNr++);
				else if (name == "texture_specular")
					number = std::to_string(specularNr++); // transfer unsigned int to stream
				else if (name == "texture_normal")
					number = std::to_string(normalNr++); // transfer unsigned int to stream
				else if (name == "texture_height")
					number = std::to_string(heightNr++); // transfer unsigned int to stream

				shader.setSampler2D((name + number).c_str(), textures[i], i);
			}

			// draw mesh
			glBindVertexArray(vertexArrayObject);
			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			// always good practice to set everything back to defaults once configured.
			glActiveTexture(GL_TEXTURE0);
		}

		void drawIstances(Shader shader, unsigned int drawMode = GL_TRIANGLES, unsigned int nIstances = 1) {
			// bind appropriate textures
			unsigned int diffuseNr = 1;
			unsigned int specularNr = 1;
			unsigned int normalNr = 1;
			unsigned int heightNr = 1;
			for (unsigned int i = 0; i < textures.size(); i++) {
				// retrieve texture number (the N in diffuse_textureN)
				std::string number;
				//std::string name = textures[i].type;
				std::string name = textures[i].type;
				if (name == "texture_diffuse") {
					// transfer unsigned int to stream
					number = std::to_string(diffuseNr++);
				}
				else if (name == "texture_specular") {
					// transfer unsigned int to stream
					number = std::to_string(specularNr++);
				}
				else if (name == "texture_normal") {
					// transfer unsigned int to stream
					number = std::to_string(normalNr++);
				}
				else if (name == "texture_height") {
					// transfer unsigned int to stream
					number = std::to_string(heightNr++);
				}

				// bind texture with custom index
				shader.setSampler2D((name + number).c_str(), textures[i], i);
			}

			// draw mesh
			glBindVertexArray(vertexArrayObject);
			//shader.use();
			glDrawElementsInstanced(drawMode, indices.size(), GL_UNSIGNED_INT, 0, nIstances);
			glBindVertexArray(0);

			// always good practice to set everything back to defaults once configured.
			glActiveTexture(GL_TEXTURE0);
		}

	private:
		//  Render data
		unsigned int _vertexBufferObject, _elementBufferObject;

	private:
		// initializes all the buffer objects/arrays
		void setupMesh() {
			// create buffers/arrays
			glGenVertexArrays(1, &vertexArrayObject);
			glGenBuffers(1, &_vertexBufferObject);
			glGenBuffers(1, &_elementBufferObject);

			glBindVertexArray(vertexArrayObject);
			// load data into vertex buffers
			glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferObject);
			// A great thing about structs is that their memory layout is sequential for all its items.
			// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
			// again translates to 3/2 floats which translates to a byte array.
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _elementBufferObject);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

			// set the vertex attribute pointers
			// vertex Positions
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
			// vertex normals
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
			// vertex texture coords
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
			// vertex tangent
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
			// vertex bitangent
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

			glBindVertexArray(0);
		}
	};
}
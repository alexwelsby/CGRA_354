
#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <filesystem>
#include "stb_image.h"
// glm
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>
#include "glm/gtx/string_cast.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include <random>

// project
#include "opengl.hpp"
#include "cgra/cgra_mesh.hpp"


// Basic model that holds the shader, mesh and transform for drawing.
// Can be copied and/or modified for adding in extra information for drawing
// including colors for diffuse/specular, and textures for texture mapping etc.
struct basic_model {
	GLuint shader = 0;
	cgra::gl_mesh mesh;
	float ambStr = 0.1;
	float diffStr = 0.5;
	float specStr = 0.5;
	float shininessStr = 1.0;
	std::vector<glm::vec3> instanceOffsets;
	std::vector<glm::vec3> instanceColors;
	std::vector<glm::mat4> instanceRotations;
	glm::vec3 ambColor{ 1, 1, 1 };
	glm::vec3 diffColor{ 1, 1, 1 };
	glm::vec3 specColor{ 1, 1, 1 };
	glm::vec3 shinColor{ 1, 0, 0 };
	glm::vec3 color{1, 0, 0};
	glm::mat4 modelTransform{1.0};

	void draw(const glm::mat4 &view, const glm::mat4 proj) {
		using namespace glm;

		// cacluate the modelview transform
		mat4 modelview = view * modelTransform;

		// load shader and variables
		glUseProgram(shader);
		glUniformMatrix4fv(glGetUniformLocation(shader, "uProjectionMatrix"), 1, false, value_ptr(proj));
		glUniformMatrix4fv(glGetUniformLocation(shader, "uModelViewMatrix"), 1, false, value_ptr(modelview));
		glUniform3fv(glGetUniformLocation(shader, "uColor"), 1, value_ptr(color));

		GLuint offsets = glGetUniformLocation(shader, "offsets");
		glUniform3fv(offsets, 100, value_ptr(instanceOffsets[0]));

		GLuint colors = glGetUniformLocation(shader, "colors");
		glUniform3fv(colors, 100, glm::value_ptr(instanceColors[0]));

		GLuint rotations = glGetUniformLocation(shader, "rotations");
		glUniformMatrix4fv(glGetUniformLocation(shader, "rotations"), 100, false, value_ptr(instanceRotations[0]));

		//phong stuff
		glUniform3fv(glGetUniformLocation(shader, "uAmbColor"), 1, value_ptr(ambColor));
		glUniform3fv(glGetUniformLocation(shader, "uDiffColor"), 1, value_ptr(diffColor));
		glUniform3fv(glGetUniformLocation(shader, "uSpecColor"), 1, value_ptr(specColor));
		glUniform3fv(glGetUniformLocation(shader, "uShinColor"), 1, value_ptr(shinColor));
		glUniform1f(glGetUniformLocation(shader, "uAmbStrength"), ambStr);
		glUniform1f(glGetUniformLocation(shader, "uDiffStrength"), diffStr);
		glUniform1f(glGetUniformLocation(shader, "uSpecStrength"), specStr);
		glUniform1f(glGetUniformLocation(shader, "uShiniStrength"), shininessStr);
		// draw the mesh
		mesh.draw(); 
	}

	void init_texture(const std::string& filepath) {

		//creating one openGL texture
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		//set texture wrapping/filtering options (on currently bound texture object)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//load/generate texture
		int width, height, nrChannels;
		unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}

		stbi_image_free(data);
	}

};
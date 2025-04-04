
#pragma once
#define GLM_ENABLE_EXPERIMENTAL
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



		for (int i = 0; i < 100; ++i) {
			int transX = (rand() % 100) - 50;
			int transY = (rand() % 100) - 50;
			int transZ = (rand() % 100) - 50;
			int distance = rand() % 101;
			std::random_device rd;  // Will be used to obtain a seed for the random number engine
			std::mt19937 gen(rd());
			std::uniform_real_distribution<> dis(1.0, 6.25);
			float r = static_cast <float>(rand()) / static_cast <float>(RAND_MAX);
			float g = static_cast <float>(rand()) / static_cast <float>(RAND_MAX);
			float b = static_cast <float>(rand()) / static_cast <float>(RAND_MAX);

			instanceColors.emplace_back(glm::vec3(r, g, b));
			instanceOffsets.emplace_back(glm::vec3(transX, transY, transZ));
			//instance rotations is a randomly generated yaw/pitch/

			r = 1.0;
			mat4 yaw = glm::rotate(mat4(1), r, vec3(0, 1, 0));
			mat4 pitch = glm::rotate(mat4(1), r, vec3(1, 0, 0));
			mat4 view = glm::translate(mat4(1), vec3(0, -5, -r));
			instanceRotations.emplace_back(glm::mat4(view * pitch * yaw));
			instanceRotations[i] *= modelTransform;

		}
		std::cout << to_string(instanceRotations[1]) << "," << to_string(modelview) << std::endl;

		GLuint offsets = glGetUniformLocation(shader, "offsets");
		glUniform3fv(offsets, 100, value_ptr(instanceOffsets[0]));

		GLuint colors = glGetUniformLocation(shader, "colors");
		glUniform3fv(colors, 100, glm::value_ptr(instanceColors[0]));

		GLuint rotations = glGetUniformLocation(shader, "rotations");
		glUniformMatrix4fv(glGetUniformLocation(shader, "rotations"), 1, false, value_ptr(instanceRotations[0]));

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
};
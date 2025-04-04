
#pragma once

// glm
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

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
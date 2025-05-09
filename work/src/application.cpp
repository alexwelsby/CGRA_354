
// std
#include <iostream>
#include <string>
#include <chrono>

// glm
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// project
#include "application.hpp"
#include "bounding_box.hpp"
#include "cgra/cgra_geometry.hpp"
#include "cgra/cgra_gui.hpp"
#include "cgra/cgra_image.hpp"
#include "cgra/cgra_shader.hpp"
#include "cgra/cgra_wavefront.hpp"


using namespace std;
using namespace cgra;
using namespace glm;

float SLIDER_LOWER_BOUND = 0.0;
float DISTANCE_UPPER_BOUND = 100.0;
float ROTATE_UPPER_BOUND = 6.25;


Application::Application(GLFWwindow *window) : m_window(window) {
	
	// build the shader for the model
	shader_builder color_sb;
	color_sb.set_shader(GL_VERTEX_SHADER, CGRA_SRCDIR + std::string("//res//shaders//default_vert.glsl"));
	color_sb.set_shader(GL_FRAGMENT_SHADER, CGRA_SRCDIR + std::string("//res//shaders//default_frag.glsl"));
	GLuint color_shader = color_sb.build();

	// build the mesh for the model
	mesh_builder teapot_mb = load_wavefront_data(CGRA_SRCDIR + std::string("//res//assets//teapot.obj"));
	gl_mesh teapot_mesh = teapot_mb.build();

	// put together an object
	m_model.shader = color_shader;
	m_model.mesh = teapot_mesh;
	m_model.color = glm::vec3(1, 0, 0);
	m_model.modelTransform = glm::mat4(1);

	for (int i = 0; i < 100; ++i) {
		int transX = (rand() % 100) - 50;
		int transY = (rand() % 100) - 50;
		int transZ = (rand() % 100) - 50;
		int distance = rand() % 101;
		std::random_device rd;  // Will be used to obtain a seed for the random number engine
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> dis(1.0, 6.25);
		std::uniform_real_distribution<float> scale_dis(0.0f, 1.0f);
		float r = static_cast <float>(rand()) / static_cast <float>(RAND_MAX);
		float g = static_cast <float>(rand()) / static_cast <float>(RAND_MAX);
		float b = static_cast <float>(rand()) / static_cast <float>(RAND_MAX);

		float yaw_angle = dis(gen);
		float pitch_angle = dis(gen);
		float scale = scale_dis(gen);

		m_model.instanceColors.emplace_back(glm::vec3(r, g, b));
		m_model.instanceOffsets.emplace_back(glm::vec3(transX, transY, transZ));
		//instance rotations is a randomly generated yaw/pitch/

		r = 1.0;
		mat4 yaw = glm::rotate(mat4(1), yaw_angle, vec3(0, 1, 0));
		mat4 pitch = glm::rotate(mat4(1), pitch_angle, vec3(1, 0, 0));
		mat4 view = glm::translate(mat4(1), vec3(0, -5, -distance));
		m_model.instanceRotations.emplace_back(glm::mat4(view * pitch * yaw * scale));
		m_model.instanceRotations[i] *= m_model.modelTransform;

	}
	m_model.init_texture("wall.jpg");

}


void Application::render() {
	
	// retrieve the window hieght
	int width, height;
	glfwGetFramebufferSize(m_window, &width, &height); 

	m_windowsize = vec2(width, height); // update window size
	glViewport(0, 0, width, height); // set the viewport to draw to the entire window

	// clear the back-buffer
	glClearColor(0.3f, 0.3f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

	// enable flags for normal/forward rendering
	glEnable(GL_DEPTH_TEST); 
	glDepthFunc(GL_LESS);

	// calculate the projection and view matrix
	mat4 proj = perspective(1.f, float(width) / height, 0.1f, 1000.f);
	mat4 yaw = glm::rotate(mat4(1), m_yaw, vec3(0, 1, 0));
	mat4 pitch = glm::rotate(mat4(1), m_pitch, vec3(1, 0, 0));
	mat4 view = translate(mat4(1), vec3(0, -5, -m_distance)); // TODO replace view matrix with the camera transform

	
	

	// draw options
	if (m_show_grid) cgra::drawGrid(view, proj);
	if (m_show_axis) cgra::drawAxis(view, proj);
	glPolygonMode(GL_FRONT_AND_BACK, (m_showWireframe) ? GL_LINE : GL_FILL);

	// draw the model
	m_model.draw(view*pitch*yaw, proj);

}


void Application::renderGUI() {

	// setup window
	ImGui::SetNextWindowPos(ImVec2(5, 5), ImGuiSetCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiSetCond_Once);
	ImGui::Begin("Camera", 0);

	// display current camera parameters
	ImGui::Text("Application %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::SliderFloat3("Model Color", value_ptr(m_model.color), 0, 1, "%.2f");
	ImGui::SliderFloat("Yaw", &m_yaw, SLIDER_LOWER_BOUND, ROTATE_UPPER_BOUND, "%.2f");
	ImGui::SliderFloat("Pitch", &m_pitch, SLIDER_LOWER_BOUND, ROTATE_UPPER_BOUND, "%.2f");
	ImGui::SliderFloat("Distance", &m_distance, SLIDER_LOWER_BOUND, DISTANCE_UPPER_BOUND, "%.1f");

	//phong shader stuff
	ImGui::SliderFloat3("Ambient Color", value_ptr(m_model.ambColor), 0, 1, "%.2f");
	ImGui::SliderFloat3("Diffuse Color", value_ptr(m_model.diffColor), 0, 1, "%.2f");
	ImGui::SliderFloat3("Specular Color", value_ptr(m_model.specColor), 0, 1, "%.2f");
	ImGui::SliderFloat("Ambient", &m_model.ambStr, 0.0, 1.0, "%.2f");
	ImGui::SliderFloat("Diffuse", &m_model.diffStr, 0.0, 1.0, "%.2f");
	ImGui::SliderFloat("Specular", &m_model.specStr, 0.0, 1.0, "%.1f");
	ImGui::SliderFloat("Shininess", &m_model.shininessStr, 0.0, 1.0, "%.1f");

	// extra drawing parameters
	ImGui::Checkbox("Show axis", &m_show_axis);
	ImGui::SameLine();
	ImGui::Checkbox("Show grid", &m_show_grid);
	ImGui::Checkbox("Wireframe", &m_showWireframe);
	ImGui::SameLine();
	if (ImGui::Button("Screenshot")) rgba_image::screenshot(true);

	// finish creating window
	ImGui::End();
}


void Application::cursorPosCallback(double xpos, double ypos) {
	
	if (m_button_pressed) {
		double deltaX = (xpos - mouse_X) * 0.01;
		double deltaY = (ypos - mouse_Y) * 0.01;
		m_yaw = std::clamp(float(m_yaw+deltaX), SLIDER_LOWER_BOUND, ROTATE_UPPER_BOUND);
		m_pitch = std::clamp(float(m_pitch + deltaY), SLIDER_LOWER_BOUND, ROTATE_UPPER_BOUND);
	}
	mouse_X = xpos;
	mouse_Y = ypos;

	(void)xpos, ypos; // currently un-used
}


void Application::mouseButtonCallback(int button, int action, int mods) {
	if (action == 1) {
		m_button_pressed = true;
	}
	else {
		m_button_pressed = false;
	}
	(void)button, action, mods; // currently un-used
}


void Application::scrollCallback(double xoffset, double yoffset) {

	m_distance += yoffset*2;

	m_distance = std::clamp(m_distance, SLIDER_LOWER_BOUND, DISTANCE_UPPER_BOUND);
	(void)xoffset, yoffset; // currently un-used
}


void Application::keyCallback(int key, int scancode, int action, int mods) {
	(void)key, (void)scancode, (void)action, (void)mods; // currently un-used
}


void Application::charCallback(unsigned int c) {
	(void)c; // currently un-used
}
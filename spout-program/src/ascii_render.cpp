#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "ascii_render.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

void ascii_render::Draw() {
	PixelsToString();
	UpdateProjection();
	shader.Bind();
	GLCall(glUniformMatrix4fv(glGetUniformLocation(shader.GetProgram(), "projection"), 1, NULL, glm::value_ptr(m_projection)));
	for (auto character : characters) {
		character.Draw();
	}
	shader.Unbind();
}

void ascii_render::UpdateImage(const cv::Mat image)
{
	m_inputImage = image;
	m_img_w = image.cols;
	m_img_h = image.rows;
}

void ascii_render::UpdateProjection() {
	int window_w, window_h = 0;
	GLCall(glfwGetWindowSize(window, &window_w, &window_h));
	m_projection = glm::ortho(0.0f, static_cast<float>(window_w), 0.0f, static_cast<float>(window_h));
}

std::string ascii_render::PixelsToString()
{
	unsigned char* pixelPtr = m_inputImage.data;
	int channels = m_inputImage.channels();
	unsigned char pixel = '\0';
	for (int i = 0; i < m_inputImage.rows; i++) {
		for (int j = 0; j < m_inputImage.cols; j++) {
			pixel = pixelPtr[i*m_inputImage.cols + j];
			std::cout << (unsigned int)pixel << std::endl;
		}
	}
	return std::string();
}

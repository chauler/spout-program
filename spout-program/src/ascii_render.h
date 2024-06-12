#pragma once
#include <memory>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include "Renderer.h"
#include "Shader.h"
#include "FontLoader.h"
#include "ft2build.h"
#include "Character.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <opencv2/core.hpp>
#include FT_FREETYPE_H

class ascii_render {
public:
	ascii_render(GLFWwindow* window) : window(window), m_charset(" .',:;clxokXdO0KN") {
		int window_w, window_h = 0;
		glfwGetWindowSize(window, &window_w, &window_h);
		m_projection = glm::ortho(0.0f, static_cast<float>(window_w), 0.0f, static_cast<float>(window_h));
		fontLoader.LoadFace("res/fonts/arial.ttf");
		FT_Face face = fontLoader.GetFace();
		//FT_Render_Glyph(face->glyph, FT_RENDER_MODE_MONO);
		shader.AddShader(GL_VERTEX_SHADER, "res/shaders/text.vs");
		shader.AddShader(GL_FRAGMENT_SHADER, "res/shaders/text.fs");
		shader.CompileShader();
		Character ch;
		ch.Init(face);
		characters.push_back(ch);
		shader.Bind();
		GLCall(glUniformMatrix4fv(glGetUniformLocation(shader.GetProgram(), "projection"), 1, NULL, glm::value_ptr(m_projection)));
		float textColor[3] = { 0.8, 0.2, 0.2 };
		GLCall(glUniform3f(glGetUniformLocation(shader.GetProgram(), "textColor"), textColor[0], textColor[1], textColor[2]));
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		shader.Unbind();
	}
	void Draw();
	void UpdateImage(const cv::Mat);
private:
	void UpdateProjection();
	std::string PixelsToString();
	std::string m_charset{};
	const char* vertexShaderSource = "";
	const char* fragShaderSource = "";
	unsigned int fragShader, vertexShader, shaderProgram = 0;
	Shader shader;
	FontLoader fontLoader;
	std::vector<Character> characters;
	unsigned int m_inputTexture;
	cv::Mat m_inputImage;
	glm::mat4 m_projection;
	unsigned int m_img_w, m_img_h = 0;
	GLFWwindow* window = nullptr;
};
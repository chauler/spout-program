#pragma once
#include <memory>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <map>
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
		glfwGetWindowSize(window, &m_win_w, &m_win_h);
		m_projection = glm::ortho(0.0f, static_cast<float>(m_win_w), 0.0f, static_cast<float>(m_win_h));
		fontLoader.LoadFace("res/fonts/arial.ttf");
		m_face = fontLoader.GetFace();
		LoadCharacterData();
		shader.AddShader(GL_VERTEX_SHADER, "res/shaders/text.vs");
		shader.AddShader(GL_FRAGMENT_SHADER, "res/shaders/text.fs");
		shader.CompileShader();
		Character ch;
		ch.Init(m_face);
		m_character = ch;
		shader.Bind();
		GLCall(glUniformMatrix4fv(glGetUniformLocation(shader.GetProgram(), "projection"), 1, NULL, glm::value_ptr(m_projection)));
		float textColor[3] = { 1.0f, 1.0f, 1.0f };
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
	void LoadCharacterData();
	std::string m_charset{};
	unsigned int fragShader, vertexShader, shaderProgram = 0;
	Shader shader;
	FontLoader fontLoader;
	std::string m_characters;
	std::map<char, CharacterData> m_characterData;
	Character m_character;
	unsigned int m_inputTexture, m_textArray;
	cv::Mat m_inputImage;
	glm::mat4 m_projection;
	unsigned int m_img_w, m_img_h = 0;
	GLFWwindow* window = nullptr;
	int m_win_w, m_win_h = 0;
	FT_Face m_face;
};

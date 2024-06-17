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
		shader.Bind();
		GLCall(glUniformMatrix4fv(glGetUniformLocation(shader.GetProgram(), "projection"), 1, NULL, glm::value_ptr(m_projection)));
		float textColor[3] = { 1.0f, 1.0f, 1.0f };
		GLCall(glUniform3f(glGetUniformLocation(shader.GetProgram(), "textColor"), textColor[0], textColor[1], textColor[2]));
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		shader.Unbind();

		GLCall(glGenFramebuffers(1, &m_FBO));
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_FBO));
		GLCall(glGenTextures(1, &m_outTex));
		GLCall(glBindTexture(GL_TEXTURE_2D, m_outTex));
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_win_w, m_win_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		GLCall(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_outTex, 0));
		GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
		GLCall(glDrawBuffers(1, DrawBuffers));
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			exit(1);
		GLCall(glGenBuffers(1, &m_VBO));
		GLCall(glGenBuffers(1, &m_EBO));
		GLCall(glGenVertexArrays(1, &m_VAO));
		GLCall(glBindVertexArray(m_VAO));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
		GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_DYNAMIC_DRAW));
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO));
		GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW));
		GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0));
		GLCall(glEnableVertexAttribArray(0));
		GLCall(glGenBuffers(1, &m_iVBO));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_iVBO));
		GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(m_positions), m_positions, GL_DYNAMIC_DRAW));
		GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0));
		GLCall(glEnableVertexAttribArray(1));
		GLCall(glVertexAttribDivisor(1, 1));
		GLCall(glBindVertexArray(0));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

	}
	unsigned int Draw();
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
	unsigned int m_VBO, m_VAO, m_EBO, m_iVBO, m_FBO, m_outTex = 0;
	vertex m_vertices[4] = {
		{0.0f,  10.0f, 15.0f},
		{0.0f, 0.0f, 15.0f},
		{10.0f, 0.0f, 15.0f},
		{10.0f,  10.0f, 15.0f}
	};
	unsigned int indices[6] = { 0, 1, 3, 1, 2, 3 };
	vertex* m_positions;
	float m_pixelSize = 10.0f;
};

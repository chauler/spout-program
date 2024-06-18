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

struct SpoutOutTex {
	unsigned int id;
	unsigned int w;
	unsigned int h;
};

class ascii_render {
public:
	ascii_render(GLFWwindow*);
	SpoutOutTex Draw();
	void UpdateImage(const cv::Mat);
	void UpdateState(float, int);
private:
	void UpdateProjection();
	std::string PixelsToString();
	void LoadCharacterData(int = 100);
	std::string m_charset{};
	Shader shader;
	FontLoader fontLoader;
	cv::Mat m_inputImage;
	unsigned int m_img_w, m_img_h = 0;
	GLFWwindow* window = nullptr;
	int m_win_w, m_win_h = 0;
	FT_Face m_face;
	unsigned int m_VBO, m_VAO, m_EBO, m_iVBO, m_FBO, m_outTex, m_textArray = 0;
	float m_charSize = 50.0f;
	vertex m_vertices[4] = {
		{0.0f,  m_charSize, 15.0f},
		{0.0f, 0.0f, 15.0f},
		{m_charSize, 0.0f, 15.0f},
		{m_charSize,  m_charSize, 15.0f}
	};
	unsigned int indices[6] = { 0, 1, 3, 1, 2, 3 };
	vertex* m_positions;
	float m_pixelSize = 10.0f;
	int m_charRes = 100;
};

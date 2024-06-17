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
	ascii_render(GLFWwindow*);
	unsigned int Draw();
	void UpdateImage(const cv::Mat);
private:
	void UpdateProjection();
	std::string PixelsToString();
	void LoadCharacterData();
	std::string m_charset{};
	Shader shader;
	FontLoader fontLoader;
	cv::Mat m_inputImage;
	unsigned int m_img_w, m_img_h = 0;
	GLFWwindow* window = nullptr;
	int m_win_w, m_win_h = 0;
	FT_Face m_face;
	unsigned int m_VBO, m_VAO, m_EBO, m_iVBO, m_FBO, m_outTex, m_textArray = 0;
	vertex m_vertices[4] = {
		{0.0f,  10.0f, 15.0f},
		{0.0f, 0.0f, 15.0f},
		{10.0f, 0.0f, 15.0f},
		{10.0f,  10.0f, 15.0f}
	};
	unsigned int indices[6] = { 0, 1, 3, 1, 2, 3 };
	vertex* m_positions;
	float m_pixelSize = 10.0f;
	float m_charSize = 10.0f;
};

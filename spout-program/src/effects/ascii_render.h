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
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <opencv2/core.hpp>
#include FT_FREETYPE_H

struct GenericGlyphData {
	float x;
	float y;
	float throwawayData;
};

struct InstanceData {
	float row;
	float column;
	float texArrayIndex;
};

struct SpoutOutTex {
	unsigned int id;
	unsigned int w;
	unsigned int h;
};

struct InputImage {
	cv::Mat image;
	unsigned int w;
	unsigned int h;
};

class ascii_render {
public:
	ascii_render(GLFWwindow*);
	SpoutOutTex Draw();
	void UpdateImage(const cv::Mat&);
	void UpdateState(float, int, glm::vec4, glm::vec4);
private:
	void UpdateProjection();
	void CalculateCharsFromLuminance();
	void LoadCharacterData(int = 100);
	Shader shader;
	FontLoader fontLoader;
	GLFWwindow* window = nullptr;
	int m_winW, m_winH = 0;
	InputImage m_inputImage;
	FT_Face m_face;
	unsigned int m_VBO = 0, m_VAO = 0, m_EBO = 0, m_iVBO = 0, m_iVBO2 = 0, m_FBO = 0, m_outTex = 0, m_textArray = 0, m_inputTex = 0;
	float m_charSize = 50.0f;
	GenericGlyphData m_vertices[4] = {
		{0.0f,  m_charSize, 15.0f},
		{0.0f, 0.0f, 15.0f},
		{m_charSize, 0.0f, 15.0f},
		{m_charSize,  m_charSize, 15.0f}
	};
	unsigned int indices[6] = { 0, 1, 3, 1, 2, 3 };
	InstanceData* m_positions;
	InstanceData* m_colors;
	float m_pixelSize = 10.0f;
	int m_charRes = 100;
	glm::vec4 m_bgColor{1.0, 0.5, 0.5, 1.0};
	glm::vec4 m_charColor{ 1.0, 1.0, 1.0, 1.0 };
	std::map<unsigned int, std::wstring> m_charSets{
		{8, L" .-+o$#8"},
		{16, L" .',:;clxokXdO0KN"},
		{32, L" `´¨·¸˜’:~‹°—÷¡|/+}?1u@VY©4ÐŽÂMÆ"}
	};
	std::wstring& m_charset;
};

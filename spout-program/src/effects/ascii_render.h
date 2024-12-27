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
#include <Quad.h>
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

struct iVec2 {
	int x;
	int y;
};

class ascii_render {
public:
	ascii_render();
	SpoutOutTex Draw(const cv::Mat&);
	void UpdateImage(const cv::Mat&);
	void UpdateState(float, glm::vec4, glm::vec4);
private:
	void LoadCharacterData(int = 100);
	Shader shader, computeShader, sobelShader, dGaussianShader, screenRenderShader;
	unsigned int m_computeShaderOutput = 0;
	FontLoader fontLoader;
	iVec2 m_prevDimensions;
	FT_Face m_face;
	unsigned int m_VBO = 0, m_VAO = 0, m_EBO = 0, m_iVBO = 0, m_iVBO2 = 0, m_edgeArray = 0,
		m_FBO = 0, m_outTex = 0, m_textArray = 0, m_inputTex = 0, m_intermediate = 0, m_intermediateFBO = 0, m_intermediate2 = 0, m_intermediateFBO2 = 0;
	float m_charSize = 8.0f;
	GenericGlyphData m_vertices[4] = {
		{0.0f,  1.0f, 15.0f},
		{0.0f, 0.0f, 15.0f},
		{1.0f, 0.0f, 15.0f},
		{1.0f, 1.0f, 15.0f}
	};
	unsigned int indices[6] = { 0, 1, 3, 1, 2, 3 };
	InstanceData* m_positions;
	int m_numChars = 16;
	glm::vec4 m_bgColor{1.0, 0.5, 0.5, 1.0};
	glm::vec4 m_charColor{ 1.0, 1.0, 1.0, 1.0 };
	std::map<unsigned int, std::wstring> m_charSets{
		{8, L" .-+o$#8"},
		//{16, L" .',:;clxokXdO0KN"},
		{16, L" ,:;clxokXdO0KNNN"},
		{32, L" `´¨·¸˜’:~‹°—÷¡|/+}?1u@VY©4ÐŽÂMÆ"}
	};
	std::wstring& m_charset;
	Quad m_fullscreenQuad;
};

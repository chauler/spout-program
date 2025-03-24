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
#include <Quad.h>
#include "effects/IEffect.h"
#include "util/types.h"
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

struct AsciiRenderState {
	int charSize;
	int numChars;
	glm::vec4 bgColor;
	glm::vec4 charColor;
	float epsilon;
	float phi;
	float sigma;
	float k;
	float p;
};

class ascii_render : public IEffect {
public:
	ascii_render();
	SpoutOutTex Draw(unsigned int);
	void UpdateState(const std::any& state) override;
	void DisplayGUIComponent() override;
private:
	void UpdateImage(unsigned int);
	void LoadCharacterData(int = 100);
	Shader shader, computeShader, sobelShader, dGaussianShader, screenRenderShader;
	unsigned int m_computeShaderOutput = 0;
	FontLoader fontLoader;
	iVec2 m_prevDimensions;
	FT_Face m_face;
	unsigned int m_VBO = 0, m_VAO = 0, m_EBO = 0, m_iVBO = 0, m_iVBO2 = 0, m_edgeArray = 0,
		m_FBO = 0, m_outTex = 0, m_textArray = 0, m_intermediate = 0, m_intermediateFBO = 0, m_intermediate2 = 0, m_intermediateFBO2 = 0;
	GenericGlyphData m_vertices[4] = {
		{0.0f,  1.0f, 15.0f},
		{0.0f, 0.0f, 15.0f},
		{1.0f, 0.0f, 15.0f},
		{1.0f, 1.0f, 15.0f}
	};
	AsciiRenderState m_config;
	unsigned int m_outputW = 0, m_outputH = 0;
	unsigned int indices[6] = { 0, 1, 3, 1, 2, 3 };
	InstanceData* m_positions;
	std::map<unsigned int, std::wstring> m_charSets{
		{8, L" .-+o$#8"},
		//{16, L" .',:;clxokXdO0KN"},
		{16, L" ,:;clxokXdO0KNNN"},
		{32, L" `´¨·¸˜’:~‹°—÷¡|/+}?1u@VY©4ÐŽÂMÆ"}
	};
	std::wstring m_charset;
	Quad m_fullscreenQuad;
};

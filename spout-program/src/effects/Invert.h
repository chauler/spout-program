#pragma once
#include "IEffect.h"
#include "Shader.h"
#include "Quad.h"
#include "util/types.h"

class Invert : public IEffect {
public:
	Invert();
	SpoutOutTex Draw(unsigned int);
	void DisplayGUIComponent() override;
private:
	void UpdateImage(unsigned int);
	Shader shader;
	unsigned int m_VBO = 0, m_VAO = 0, m_EBO = 0, m_FBO = 0, m_outTex = 0;
	Quad m_fullscreenQuad;
	uVec2 m_prevDimensions;
};
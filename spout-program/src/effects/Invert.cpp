#include "Invert.h"
#include "Renderer.h"
#include "util/util.h"
#include "imgui/imgui.h"

Invert::Invert() :
	m_fullscreenQuad(),
	m_prevDimensions({ 0, 0 })
{
	shader.AddShader(GL_VERTEX_SHADER, SpoutProgram::Util::ReadFile("res/shaders/invert.vs"));
	shader.AddShader(GL_FRAGMENT_SHADER, SpoutProgram::Util::ReadFile("res/shaders/invert.fs"));
	shader.CompileShader();

	GLCall(glGenFramebuffers(1, &m_FBO));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_FBO));
	GLCall(glGenTextures(1, &m_outTex));
	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_outTex));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLCall(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_outTex, 0));
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	GLCall(glDrawBuffers(1, DrawBuffers));
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		exit(1);
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

SpoutOutTex Invert::Draw(unsigned int imageID) {
	unsigned int cols, rows;
	GLCall(glBindTexture(GL_TEXTURE_2D, imageID));
	GLCall(glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, (GLint*)&cols));
	GLCall(glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, (GLint*)&rows));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));

	//If incoming texture is invalid, don't update anything and just return the previous result
	if (cols == 0 || rows == 0) {
		return { m_outTex, (unsigned int)cols, (unsigned int)rows };
	}

	UpdateImage(imageID);

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_FBO));
	GLCall(glViewport(0, 0, cols, rows));
	GLCall(glClear(GL_COLOR_BUFFER_BIT));
	shader.Bind();
	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_2D, imageID));
	m_fullscreenQuad.Draw();
	shader.Unbind();
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	return { m_outTex, (unsigned int)cols, (unsigned int)rows };
}

void Invert::DisplayGUIComponent() {
}

void Invert::UpdateImage(unsigned int imageID) {
	unsigned int cols, rows;
	GLCall(glBindTexture(GL_TEXTURE_2D, imageID));
	GLCall(glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, (GLint*)&cols));
	GLCall(glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, (GLint*)&rows));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));

	if (cols != m_prevDimensions.x || rows != m_prevDimensions.y) {
		m_prevDimensions.x = cols;
		m_prevDimensions.y = rows;

		GLCall(glActiveTexture(GL_TEXTURE0));
		GLCall(glBindTexture(GL_TEXTURE_2D, m_outTex));
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, cols, rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0));
	}

	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
	GLCall(glActiveTexture(GL_TEXTURE0));
}
